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
    mm_init_kmap(); 

    kernel_root_ptb = (struct pgtable *) alloc_zero_page();
    if (!kernel_root_ptb) panic("Não foi possível alocar a root PTB");

    #define MAP_REGION(start, end, perm) \
        for (u64 p = (u64)PPN_DOWN(start) << PAGE_SHIFT; \
             p < (u64)PPN_UP(end) << PAGE_SHIFT; \
             p += PAGE_SIZE) \
            vm_map_page(p, p, perm);

    MAP_REGION(kmap.text_start, kmap.text_end, PTE_READ | PTE_EXEC);
    MAP_REGION(kmap.rodata_start, kmap.rodata_end, PTE_READ);
    MAP_REGION(kmap.data_start, kmap.data_end, PTE_READ | PTE_WRITE);
    MAP_REGION(kmap.bss_start, kmap.bss_end, PTE_READ | PTE_WRITE);

    u64 ram_start = (u64)PPN_UP(kmap.kernel_end) << PAGE_SHIFT;
    for (u64 p = ram_start; p < 0x88000000UL; p += PAGE_SIZE)
        vm_map_page(p, p, PTE_READ | PTE_WRITE);

    vm_map_page(0x10000000, 0x10000000, PTE_READ | PTE_WRITE);
    vm_map_page(0x0, 0x0, 0);
}

int vm_map_page(u64 va, phys_addr_t pa, u64 perm)
{
    if (!IS_ALIGNED(va) || !IS_ALIGNED(pa)) {
        return -1;
    }

    u64 mask_63_38 = ~((1UL << 38) - 1);
    u64 top_bits = va & mask_63_38;
    if (top_bits != 0 && top_bits != mask_63_38) {
        return -1;
    }

    struct pgtable *ptb = kernel_root_ptb;

    for (u64 level = 2; level > 0; level--) {
        u64 index = va_get_index(va, level);
        pte_t *ptep = ptb_get_ptep(ptb, index);

        if (!pte_valid(*ptep)) {
            phys_addr_t new_ptb_pa = alloc_zero_page();
            if (!new_ptb_pa) return -1;

            *ptep = pte_from_ppn(phys_to_ppn(new_ptb_pa)) | PTE_VALID;
        }

		if (pte_leaf(*ptep)) return -1;

        ptb = pte_next_ptb(*ptep);
    }

    u64 index0 = va_get_index(va, 0);
    pte_t *ptep0 = ptb_get_ptep(ptb, index0);

    if (pte_valid(*ptep0)) return -1;

    *ptep0 = pte_from_ppn(phys_to_ppn(pa)) | perm | PTE_VALID;

    return 0;
}

void vm_init()
{
	mm_init();

	/* uncomment the line below to load the page tables by writing the address
	 * of kernel_root_ptb to the satp CSR */
	csr_write(CSR_SATP, CSR_SATP_MODE_SV39 | phys_to_ppn((phys_addr_t) kernel_root_ptb));
}
