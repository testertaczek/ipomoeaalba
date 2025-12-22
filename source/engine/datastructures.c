#include <ia/datastructures/mpmc.h>

/* sequence values:
 * slot empty => seq == pos
 * slot full  => seq == pos + 1
 * after pop  => seq == pos + mask + 1 */
bool ia_mpmc_rotate(
    ia_mpmc        *mpmc,
    atomic_isize   *in_or_out,
    isize const     pos_delta,
    isize          *out_pos)
{
    atomic_isize *sequence;
    isize at, seq, diff, pos = ia_atomic_read_monotonic(in_or_out);

    for (;;) {
        at = pos & mpmc->mask;
        sequence = &mpmc->sequence[at];
        seq = ia_atomic_read(sequence, ia_atomic_model_acquire);
        diff = (isize)((iptr)seq - (iptr)(pos + pos_delta));

        if (diff == 0) {
            if (IA_LIKELY(ia_atomic_cmpxchg_weak_monotonic(in_or_out, &pos, pos + 1))) {
                *out_pos = pos;
                return true;
            }
        } else if (diff < 0) {
            /* it's empty */
            return false;
        } else {
            ia_cpu_relax();
            pos = ia_atomic_read_monotonic(in_or_out);
        }
    }
    IA_UNREACHABLE;
}
