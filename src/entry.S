.section .init

.option norvc

.type start, @function
.global start
start:
    .cfi_startproc

.option push
.option norelax
    la gp, global_pointer
.option pop

    # reset satp (supervisor address translation protection)
    csrw satp, zero
    # setup stack
    la sp, stack_top

    # clear .bss
    la t5, _bss_start
    la t6, _bss_end

bss_clear:
    sd zero, (t5)
    addi t5, t5, 8
    bltu t5, t6, bss_clear

    la t0, mmode_startup
    csrw mepc, t0

    # save flattened device tree address
save_dtb:
    .option push
    .option norelax
    .extern kernel_dtb
        la t0, kernel_dtb
        sw a1, (t0)
    .option pop

    # jump to kernel
    tail mmode_startup

    .cfi_endproc

.end
