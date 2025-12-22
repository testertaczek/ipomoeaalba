#pragma once
/** @file ia/base/defer.h
 *  @brief Defer statement implemented using computed goto.
 *
 *  The solution for the defer has been somewhat inspired by this blog post:
 *  [The Defer Technical Specification: It Is Time]
 *  https://thephd.dev/c2y-the-defer-technical-specification-its-time-go-go-go
 *
 *  Important notes:
 *  - The parameter to ia_defer() can be arbitrary in-scope code. It's defined 
 *    as a variable argument list, so the preprocessor shouldn't be angry.
 *  - Defer blocks only run if they are dynamically reached.
 *  - Any such defer block is only ever run a single time.
 *  - Defer blocks run in reverse order in which they were reached.
 *  - You MUST NOT jump out of whatever code is put inside the defer statement,
 *    there is no guard against it. Yielding the fiber is fine.
 *  - The return statement runs AFTER all reached defer blocks run. This also 
 *    means, if any computation is done directly on the return, it will run 
 *    AFTER all defer blocks were evaluated.
 *  - Function-scope-exit defer block is implemented via computed goto (GNU extension).
 *  - Scope-exit defer is implemented via cleanup attribute (GNU extension). These can't 
 *    guarantee reverse-order with function-scope defer blocks, so they should be used
 *    only for inner-scope blocks inside the function (DON'T MIX BOTH IN ONE SCOPE!!).
 *  - The implementation proposed here is not compatible with MSVC, but works for GCC and Clang.
 *
 *  Each defer block statically declares it's own stack-allocated record in a linked list.
 *  The deferred return (and longjmp) statements ensure the defer blocks run before actually
 *  executing return (and longjmp). The implementation makes use of computed goto statements.
 *  They are a GNU extension. Goto in C targets a fixed, static label that will translate into 
 *  a jump instruction to a fixed offset known at compile time, which will be incredible cheap.
 *  Here though, while we create jump labels associated with each defer statement statically,
 *  we only want to jump to things if they're triggered. So which labels we want to jump to 
 *  (and in what order) is dynamic. Computed goto allows us to handle that by giving two primitives:
 *
 *  1. The ability to get the address of any in-scope label that we want to use as a jump target.
 *     This is done with the && operator. The type of the label is simply void *.
 *
 *  2. The ability to give goto a memory location from which to dynamically read the jump target.
 *     This is done using the C dereference operator (*), effectively telegraphing the argument 
 *     is not the jump target, but where to LOOK for the jump target.
 *
 *  The longjmp call allows for `goto`-like functionality that can cross function boundaries,
 *  and essentially works by saving register state (at the point of a setjmp), and then restoring
 *  it all with the longjmp, which is significant work compared to a few gotos. This approach with 
 *  computed goto is much more ligthweight. The longjmp is actually similar to what we're doing 
 *  with spawn_fcontext and jump_fcontext for the fiber implementation.
 *
 *  We do wrap longjmp, since it's commonly used as the basis of more heavy-weight exception
 *  handling mechanisms. The biggest challenge with such is often doing the cleanup while unwinding
 *  the stack. Wrapping longjmp doesn't directly help a lot there, as it only would run defer blocks 
 *  when raising an exception. However, it's easy to trigger `defer` calls at the point where setjmp 
 *  gets back to execution after a jump completes.
 *
 *  While the implementation is conceptually simple and doesn't take much code, it does require 
 *  some C wizardry to understand:
 *
 *  1. C's token pasting operator (## in magic glue macro) allows us to create variables
 *     and jump labels on the fly.
 *
 *  2. We can make labels and variables unique per-statement by pasting together a prefix
 *     with the value of the LINE macro (done with an indirect macro here).
 *
 *  3. Wrapping defer code in an `if (false) {}` block allows us to skip it on the first run.
 *     But if we define a label inside the block, then push that label's address onto the stack,
 *     it can be popped off the block and jumped to.
 *
 *  4. After the user's defer code (but still inside the `if (false) {}` block), we need to add 
 *     code to check if there's another defer block we have reached, that we need to run. We always
 *     keep a linked list of labels to see the successor, if any.
 *
 *  5. When there are no more defer blocks to run, we need to jump to the actual return statement 
 *     (or longjmp). A function can have many return statements. To address that, there's a single 
 *     variable (added via `ia_defer_begin`) that gets the jump target to complete the exit. So when 
 *     the stack empties, we jump wherever that is, really. We use the same token-pasting approach 
 *     to create the label, and a computed goto to jump to it.
 *
 *  The `ia_defer_work` wraps around GNU attribute `cleanup` that let's us run a `ia_work_fn` signature
 *  function after exiting the current scope. This is our only way of supporting scope-grained defer,
 *  it's limited to executing a function instead of running any arbitrary in-scope code, and has no 
 *  order determinimsm against the computed-goto mechanism. Because of this, it's only use is for 
 *  nested scopes or really small functions that might benefit from this approach (e.g. simple file 
 *  read/write). Any "work" function can be executed within the implications of this mechanism.
 */
#include <ia/base/targets.h>
#include <ia/base/magic.h>

#define _IA_DEFER_LABEL(x) \
    IA_MAGIC_GLUE2(__defer_label_, x)
#define _IA_DEFER_NODE(x) \
    IA_MAGIC_GLUE2(__defer_node_, x)

/** If `ia_defer` is used, this must be called before any defer blocks.
 *  Has no implications for `ia_defer_work` via GNU cleanup attribute. */
#define ia_defer_begin \
    void *__defer_stack = nullptr; \
    void *__defer_return_label = nullptr

/** Can execute any arbitrary block of in-scope code, runs in reverse order to other defer blocks. */
#define ia_defer(...)                               \
    void *_IA_DEFER_NODE(__LINE__) = __defer_stack; \
    __defer_stack = && _IA_DEFER_LABEL(__LINE__);   \
    if (IA_UNLIKELY(false)) {                       \
        _IA_DEFER_LABEL(__LINE__) :                 \
        __VA_ARGS__                                 \
        if (_IA_DEFER_NODE(__LINE__) == nullptr) {  \
            goto *(__defer_return_label);           \
        }                                           \
        goto *(_IA_DEFER_NODE(__LINE__));           \
    }

/** Executes all dynamically reached defer blocks before returning. */
#define ia_defer_return \
    __defer_return_label = &&_IA_DEFER_LABEL(__LINE__); \
    if (__defer_stack) { goto *(__defer_stack); } \
    _IA_DEFER_LABEL(__LINE__) : return    

/** Executes all dynamically reached defer blocks before longjmp. */
#define ia_defer_longjmp(jump_env, jump_passed_state) \
    __defer_return_label = &&_IA_DEFER_LABEL(__LINE__); \
    if (__defer_stack) { goto *(__defer_stack); } \
    _IA_DEFER_LABEL(__LINE__) : longjmp(jump_env, jump_passed_state)

/** Defines an `ia_work_fn` prototype for deferred in-scope execution, T must be a pointer type. */
#define IA_DEFER_WORK_FN(fn, T) \
    IA_FORCE_INLINE void fn##_deferred(T *p) { if (*p) fn(*p); }

/** Executes deferred work at scope-exit. Should be used like this:
 *      FILE *f ia_defer_work(fclose) = fopen("data.txt", "r"); */
#define ia_defer_work(fn) \
    __attribute__((cleanup(fn##_deferred)))
