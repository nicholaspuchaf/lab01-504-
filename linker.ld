ENTRY(start);

/*
  The special linker variable dot `.' always contains the current output location counter.
  https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html
*/
. = 0x80000000;

SECTIONS {
    PROVIDE(_start = .);

    /* include entry point at the start of the binary */
    .text : ALIGN(4K) {
        PROVIDE(_text_start = .);
        *(.init);
        *(.text);
        PROVIDE(_text_end = .);
    }

    .bss : ALIGN(4K) {
        PROVIDE(_bss_start = .);
        *(.bss);
        *(.sbss);
        /* define a 32KB stack */
        . += 32K;
        PROVIDE(stack_top = .);
        /* define a 4096-byte space for global vars */
        . += 4096;
        PROVIDE(global_pointer = .);
        PROVIDE(kernel_dtb = .);
        PROVIDE(bootargs = .);
        PROVIDE(_bss_end = .);
    }

    .rodata : ALIGN(4K) {
        PROVIDE(_rodata_start = .);
        *(.rodata);
        *(.rodata.*);
        *(.srodata);
        PROVIDE(_rodata_end = .);
    }

    .data : ALIGN(4K) {
        PROVIDE(_data_start = .);
        *(.data);
        *(.sdata);
        PROVIDE(_data_end = .);
    }

    /*
     *  These are special compiler-generated sections that we don't really
     *  care about, but place at the end of the executable so it doesn't interfere
     *  with other sections
     */

    .eh_frame : { *(.eh_frame); }
    .comment : { *(.comment); }
    .riscv.attributes : { *(.riscv.attributes); }
    .symtab : { *(.symtab); }
    .shstrtab : { *(.shstrtab); }
    .strtab : { *(.strtab); }

    /*
     *  Ensure that the kernel image is page-aligned (makes things easier
     *  when setting up a linear mapping for physical memory)
     */
    . = ALIGN(4K);
    PROVIDE(_end = .);
}
