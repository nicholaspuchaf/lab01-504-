#ifndef __MM_H__
#define __MM_H__

#include <kernel/types.h>
#include <arch/pgtable.h>

extern char _start[];
extern char _end[];
extern char _text_start[];
extern char _text_end[];
extern char _bss_start[];
extern char _bss_end[];
extern char _rodata_start[];
extern char _rodata_end[];
extern char _data_start[];
extern char _data_end[];

/**
 * struct memory_map_t - Contains the physical address of the kernel's sessions.
 * @kernel_start: The first address of the kernel -- page alligned.
 * @kernel_end: The last address reserved for the kernel -- page alligned.
 * @kernel_size: Total size for the kernel.
 * @*_start: The first address of the session -- page alligned.
 * @*_end: The last writed address of the session -- not necessarily alligned.
 *
 * Retrieve the addresses mapped by the linker to each session of the kernel.
 * The *_start is always page alligned and the end contains the last initialized
 * address of the session (not necessarily page alligned).
 */
struct memory_map_t {
	phys_addr_t kernel_start;
	phys_addr_t kernel_end;
	size_t kernel_size;
	phys_addr_t text_start;
	phys_addr_t text_end;
	phys_addr_t bss_start;
	phys_addr_t bss_end;
	phys_addr_t rodata_start;
	phys_addr_t rodata_end;
	phys_addr_t data_start;
	phys_addr_t data_end;
};

/**
 * mm_init_kmap() - Initialize the kernel memory layout map
 *
 * This initializes the global kmap struct containing information about
 * the kernel memory layout, such as the kernel start/end addresses and size,
 * as well as the start/end of each section of the kernel.
 */
void mm_init_kmap();

/**
 * mm_init() - Initialize the memory management.
 *
 * At this first implementation we are going to map the physical addresses
 * to the virtual addresses of the same number.
 *
 * Map all the kernel sessions using the suitable flags for each session,
 * map the serial to address 0x10000000, map all the remaining addresses to
 * read and write pages
 */
void mm_init();

/**
 * vm_init() - Initialize the memory abstraction.
 *
 * Initialize the memory management and write the root PTB to the satp register
 */
void vm_init();

/**
 * vm_map_page() - Map a single virtual address page to a physical address page.
 * @va: Virtual address
 * @pa: Physical address
 * @flags: PTE flags
 *
 * Example:
 *
 *	int ret;
 *	ret = vm_map_page(0x10000, 0x10000, PTE_READ | PTE_WRITE);
 *	if (ret)
 *		panic("failed to map page!\n");
 *
 * Return: 0 on success and -1 on failure.
 */
int vm_map_page(u64 va, phys_addr_t pa, u64 flags);

#endif
