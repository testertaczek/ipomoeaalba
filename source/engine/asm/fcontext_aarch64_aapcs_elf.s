/** Aarch64 Linux/Unix (ARM64 AAPCS ELF): fiber context implementation
 * 
 *  offset | registers
 * -------------------------------------------
 *   0x00  | d8
 *   0x08  | d9
 *   0x10  | d10
 *   0x18  | d11
 *   0x20  | d12
 *   0x28  | d13
 *   0x30  | d14
 *   0x38  | d15
 *   0x40  | x19
 *   0x48  | x20
 *   0x50  | x21
 *   0x58  | x22
 *   0x60  | x23
 *   0x68  | x24
 *   0x70  | x25
 *   0x78  | x26
 *   0x80  | x27
 *   0x88  | x28
 *   0x90  | FP
 *   0x98  | LR
 *   0xa0  | PC
 *   0xa8  | align 
 */
.file "fcontext_aarch64_aapcs_elf.s"
.text

.align  2
.global jump_fcontext
.hidden jump_fcontext
.type   jump_fcontext, @function
jump_fcontext:
    /* prepare stack for GP + FPU */
    sub     sp, sp, #0xb0

/* Because GAS may save integer registers in fp registers across a function 
 * call we cannot skip saving the fp registers. Do not reinstate this test 
 * unless you fully understand what you are doing. (I don't)
 * [test if fpu env should be preserved]
 *
 * cmp  w3, #0
 * b.eq 1f
 */
    /* save d8 - d15 */
    stp     d8,  d9,  [sp, #0x00]
    stp     d10, d11, [sp, #0x10]
    stp     d12, d13, [sp, #0x20]
    stp     d14, d15, [sp, #0x30]
1:
    /* save x19-x30 */
    stp     x19, x20, [sp, #0x40]
    stp     x21, x22, [sp, #0x50]
    stp     x23, x24, [sp, #0x60]
    stp     x25, x26, [sp, #0x70]
    stp     x27, x28, [sp, #0x80]
    stp     x29, x30, [sp, #0x90]

    /* save LR as PC */
    str     x30, [sp, #0xa0]

    /* store RSP (pointing to context-data) in first argument (x0).
     * STR cannot have sp as a target register */
     mov    x4, sp

     /* restore RSP (pointing to context-data) from A2 (x1) */
     mov    sp, x0

/* [test if fpu env should be preserved]
 * cmp  w3, #0
 * b.eq 2f
 */
    /* load d8 - d15 */
    ldp     d8,  d9,  [sp, #0x00]
    ldp     d10, d11, [sp, #0x10]
    ldp     d12, d13, [sp, #0x20]
    ldp     d14, d15, [sp, #0x30]
2:
    /* load x19-x30 */
    ldp     x19, x20, [sp, #0x40]
    ldp     x21, x22, [sp, #0x50]
    ldp     x23, x24, [sp, #0x60]
    ldp     x25, x26, [sp, #0x70]
    ldp     x27, x28, [sp, #0x80]
    ldp     x29, x30, [sp, #0x90]

    /* use third argument as return value after jump
     * and as first argument in context function */
    mov     x0, x2

    /* load pc */
    ldr     x4, [sp, #0xa0]

    /* restore stack from GP + FPU */
    add     sp, sp, #0xb0

    ret x4
.size jump_fcontext,.-jump_fcontext

.align  16
.global spawn_fcontext
.hidden spawn_fcontext
.type   spawn_fcontext, @function
spawn_fcontext:
    /* shift address in x0 (allocated stack) to lower 16 byte boundary */
    and     x0, x0, ~0xf

    /* reserve space for context-data on context-stack */
    sub     x0, x0, #0xb0

    /* third arg of spawn_fcontext() == address of context-function 
     * store address as a PC to jump in */
    str     x2, [x0, #0xa0]

    /* save address of finish as return-address for context-function
     * will be entered after context-function returns (LR register) */
    adr     x1, finish
    str     x1, [x0, #0x98]

    /* return pointer to context-data (x0) */
    ret     x30

finish:
    /* exit code is zero */
    mov     x0, #0

    /* exit application */
    bl      _exit
.size spawn_fcontext,.-spawn_fcontext

/* mark that we don't need executable stack */
.section .note.GNU-stack,"",%progbits
