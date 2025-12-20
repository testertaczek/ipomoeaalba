/** RISC-V 64-bit Linux/Unix (RV64GC SysV ELF): fiber context implementation
 * 
 *  offset | registers
 * -------------------------------------------
 *   0x00  | fs0
 *   0x08  | fs1
 *   0x10  | fs2
 *   0x18  | fs3
 *   0x20  | fs4
 *   0x28  | fs5
 *   0x30  | fs6
 *   0x38  | fs7
 *   0x40  | fs8
 *   0x48  | fs9
 *   0x50  | fs10
 *   0x58  | fs11
 *   0x60  | s0 
 *   0x68  | s1 
 *   0x70  | s2 
 *   0x78  | s3 
 *   0x80  | s4 
 *   0x88  | s5 
 *   0x90  | s6 
 *   0x98  | s7 
 *   0xa0  | s8 
 *   0xa8  | s9 
 *   0xb0  | s10
 *   0xb8  | s11
 *   0xc0  | ra
 *   0xc8  | pc
 */
.file "fcontext_rv64gc_sysv_elf.s"
.text

.align  1
.global jump_fcontext
.hidden jump_fcontext
.type   jump_fcontext, @function
jump_fcontext:
    /* prepare stack for GP + FPU */
    addi    sp, sp, -0xd0

    /* save fs0 - fs11 */
    fsd     fs0,  0x00(sp)
    fsd     fs1,  0x08(sp)
    fsd     fs2,  0x10(sp)
    fsd     fs3,  0x18(sp)
    fsd     fs4,  0x20(sp)
    fsd     fs5,  0x28(sp)
    fsd     fs6,  0x30(sp)
    fsd     fs7,  0x38(sp)
    fsd     fs8,  0x40(sp)
    fsd     fs8,  0x48(sp)
    fsd     fs10, 0x50(sp)
    fsd     fs11, 0x58(sp)

    /* save s0 - s11, ra */
    sd      s0,  0x60(sp)
    sd      s1,  0x68(sp)
    sd      s2,  0x70(sp)
    sd      s3,  0x78(sp)
    sd      s4,  0x80(sp)
    sd      s5,  0x88(sp)
    sd      s6,  0x90(sp)
    sd      s7,  0x98(sp)
    sd      s8,  0xa0(sp)
    sd      s9,  0xa8(sp)
    sd      s10, 0xb0(sp)
    sd      s11, 0xb8(sp)
    sd      ra,  0xc0(sp)

    /* save RA as PC */
    sd      ra, 0xc8(sp)

    /* store SP (pointing to context-data) in A2 */
    mv      a2, sp

    /* restore SP (pointing to context-data) from A0 */
    mv      sp, a0

    /* load fs0 - fs11 */
    fld     fs0,  0x00(sp)
    fld     fs1,  0x08(sp)
    fld     fs2,  0x10(sp)
    fld     fs3,  0x18(sp)
    fld     fs4,  0x20(sp)
    fld     fs5,  0x28(sp)
    fld     fs6,  0x30(sp)
    fld     fs7,  0x38(sp)
    fld     fs8,  0x40(sp)
    fld     fs8,  0x48(sp)
    fld     fs10, 0x50(sp)
    fld     fs11, 0x58(sp)

    /* load s0 - s11, ra */
    ld      s0,  0x60(sp)
    ld      s1,  0x68(sp)
    ld      s2,  0x70(sp)
    ld      s3,  0x78(sp)
    ld      s4,  0x80(sp)
    ld      s5,  0x88(sp)
    ld      s6,  0x90(sp)
    ld      s7,  0x98(sp)
    ld      s8,  0xa0(sp)
    ld      s9,  0xa8(sp)
    ld      s10, 0xb0(sp)
    ld      s11, 0xb8(sp)
    ld      ra,  0xc0(sp)

    /* return first argument passed in context function
     * a0 == fcontext, a1 == data */
    mv      a0, a2

    /* load pc */
    ld      a2, 0xc8(sp)

    /* restore stack from GP + FPU */
    addi    sp, sp, 0xd0

    jr      a2
.size jump_fcontext,.-jump_fcontext

.align  1
.global spawn_fcontext
.hidden spawn_fcontext
.type   spawn_fcontext, @function
spawn_fcontext:
    /* shift address in a0 (allocated stack) to lower 16 byte boundary */
    andi    a0, a0, ~0xf

    /* reserve space for context-data on context-stack */
    addi    a0, a0, -0xd0

    /* third arg of spawn_fcontext() == address of context-function 
     * store address as a PC to jump in */
    sd      a2, 0xc8(a0)

    /* save address of finish as return-address for context-function
     * will be entered after context-function returns (RA register) */
    lla     a4, finish
    sd      a4, 0xc0(a0)

    /* return pointer to context-data (a0) */
    ret 

finish:
    /* exit code is zero */
    li      a0, 0

    /* exit application */ 
    tail    _exit@plt
.size spawn_fcontext,.-spawn_fcontext

/* mark that we don't need executable stack */
.section .note.GNU-stack,"",%progbits
