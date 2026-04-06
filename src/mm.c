#include <arch/pgtable.h>
#include <arch/csr.h>
#include <stddef.h>
#include <kernel/printf.h>
#include <kernel/bits.h>
#include <kernel/types.h>
#include <kernel/defs.h>
#include <kernel/string.h>
#include <kernel/mm.h>
#include <kernel/alloc.h>
#include <kernel/panic.h>
#include <kernel/sizes.h>

struct memory_map_t kmap;

void mm_init_kmap() {
	debug("reading kernel memory layout map...\n");
	kmap.kernel_start = (phys_addr_t) _start;
	kmap.kernel_end = (phys_addr_t) _end;
	kmap.kernel_size = kmap.kernel_end - kmap.kernel_start;
	kmap.text_start = (phys_addr_t) _text_start;
	kmap.text_end = (phys_addr_t) _text_end;
	kmap.bss_start = (phys_addr_t) _bss_start;
	kmap.bss_end = (phys_addr_t) _bss_end;
	kmap.rodata_start = (phys_addr_t) _rodata_start;
	kmap.rodata_end = (phys_addr_t) _rodata_end;
	kmap.data_start = (phys_addr_t) _data_start;
	kmap.data_end = (phys_addr_t) _data_end;

	debug("kernel_start = 0x%x\n", kmap.kernel_start);
	debug("kernel_end = 0x%x\n", kmap.kernel_end);
	debug("kernel_size = 0x%x\n", kmap.kernel_size);
	debug("text_start = 0x%x\n", kmap.text_start);
	debug("text_end = 0x%x\n", kmap.text_end);
	debug("bss_start = 0x%x\n", kmap.bss_start);
	debug("bss_end = 0x%x\n", kmap.bss_end);
	debug("rodata_start = 0x%x\n", kmap.rodata_start);
	debug("rodata_end = 0x%x\n", kmap.rodata_end);
	debug("data_start = 0x%x\n", kmap.data_start);
	debug("data_end = 0x%x\n", kmap.data_end);

	debug("\n");
}

struct pgtable *kernel_root_ptb;

void mm_init()
{
	/* FIXME: implement mm_init */
}

int vm_map_page(u64 va, phys_addr_t pa, u64 flags)
{
	info("va: 0x%x, pa: 0x%x, flags: 0x%x not used\n", va, pa, flags);
	/* FIXME: implement vm_map_page */
	return -1;
}

void vm_init()
{
	mm_init();

	/* uncomment the line below to load the page tables by writing the address
	 * of kernel_root_ptb to the satp CSR */
	//csr_write(CSR_SATP, CSR_SATP_MODE_SV39 | phys_to_ppn((phys_addr_t) kernel_root_ptb));
}
