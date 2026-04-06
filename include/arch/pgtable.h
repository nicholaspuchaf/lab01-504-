#ifndef __PGTABLE_H__
#define __PGTABLE_H__

#include <kernel/types.h>
#include <kernel/bits.h>
#include <kernel/defs.h>
#include <kernel/printf.h>
#include <kernel/panic.h>

/* 
 * RISC-V Sv39 virtual memory layout
 *
 * Virtual address:
 *    [ VPN[2] ]  [ VPN[1] ]  [ VPN[0] ]  [ page offset ]
 *   38  ... 30 29  ...  21 20  ...  12 11      ...    0
 *
 * Physical address:
 *    [ PPN[2] ]  [ PPN[1] ]  [ PPN[0] ]  [ page offset ]
 *   55  ... 30 29  ...  21 20  ...  12 11      ...    0
 * 
 * Page table entry
 *    [ ... ]  [ PPN[2] ]  [ PPN[1] ]  [ PPN[0] ]  [ RSW ] [D] [A] [G] [U] [X] [W] [R] [V]
 *    63   54 53  ...  28 27  ...  19 18  ...  10  9 ... 8  7   6   5   4   3   2   1   0
 *
 */

#define PAGE_SHIFT 0UL					/* 12-bit page offset field */
#define PAGE_SIZE  0UL					/* 2**12 = 4096 byte pages */
#define PAGE_MASK  0UL					/* 0xfffffffffffff000 */

/**
 * IS_ALIGNED(): Macro to check if a given address is page-aligned.
 * @addr: The address to check.
 *
 * Return: 1 if the address is aligned and 0 otherwise.
 */
#define IS_ALIGNED(addr) (((u64) (addr) & 0UL))

/**
 * PPN_DOWN(): Round address down to the nearest page boundary.
 * @addr: Address to round down.
 *
 * For example, PPN_DOWN(0x1001) == 0x1
 *
 * Returns: the PPN of the nearest page boundary, rounded down.
 */
#define PPN_DOWN(addr) ((u64) (addr) & 0UL)

/**
 * PPN_UP(): Round address up to the nearest page boundary.
 * @addr: Address to round up.
 *
 * For example, PPN_DOWN(0x1001) == 0x2
 *
 * Returns: the PPN of the nearest page boundary, rounded up.
 */
#define PPN_UP(addr) ((u64) (addr) & 0UL)

/* Helper macros for extractings page table indices */

#define VPN_SHIFT_L2 0UL			/* VPN[2] = VA[38:30] */
#define VPN_MASK_L2  0UL
#define VPN_SHIFT_L1 0UL			/* VPN[1] = VA[29:21] */
#define VPN_MASK_L1  0UL
#define VPN_SHIFT_L0 0UL			/* VPN[0] = VA[20:12] */
#define VPN_MASK_L0  0UL

/**
 * va_get_index(): Extract the index within a pagetable from a virtual address.
 * @va: Virtual address to extract the index from.
 * @level: Page table level.
 *
 * Returns: The index of VA @va within a page table of level @level.
 */
__always_inline u64 va_get_index(u64 va, u64 level)
{
	info("va: 0x%x and level: 0x%x variables not used\n", va, level);
	return 0UL;
}

#define PTB_LEVELS      0UL				/* total PTB levels */
#define PTB_INDEX_BITS  0UL				/* number of bits to map each index */
#define PTE_SHIFT       0UL				/* pagetable entry shift/offset */
#define PTE_PPN_MASK    0UL				/* mask to get only PPN from a pte */
#define PTB_ENTRY_COUNT 0UL				/* the number of mappable entries is 2**n_bits in va */

/**
 * Page table typedefs
 *
 * pte_t: page table entry (one line of a page table)
 * ppn_t: physical page number (such as the PPN field in a PTE)
 *
 * Both are u64, but this definition of a type serves as a hint for the developer.
 */
typedef u64	pte_t;
typedef u64	ppn_t;

/**
 * struct pgtable - A page table represented as an array of PTEs.
 * @entries: array of PTEs
 *
 * This struct is used to represent a page table in memory. It is not
 * meant to be allocated, but rather typecasted to an address. For example:
 *
 *	struct pgtable *ptb;
 *	ptb = (struct pgtable *) alloc_get_page();
 */
struct pgtable {
	pte_t entries[PTB_ENTRY_COUNT];	/* 512 entries */
};

/**
 * Inline function helpers for manipulating PTBs, PTEs and PPNs
 */

/**
 * phys_to_ppn() - Extract the PPN from a physical address.
 */
static __always_inline ppn_t phys_to_ppn(phys_addr_t phys)
{
	info("phys: 0x%x variable not used\n", phys);
	return 0UL;
}

/**
 * ppn_to_phys() - Convert a PPN to a physical address.
 */
static __always_inline phys_addr_t ppn_to_phys(ppn_t ppn)
{
	info("ppn: 0x%x variable not used\n", ppn);
	return 0UL;
}

/**
 * pte_get_ppn() - Extract the PPN from a PTE.
 */
static __always_inline ppn_t pte_get_ppn(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return 0UL;
}

/*
 * pte_from_ppn() - Construct a PTE from a given PPN.
 * @ppn: Physical page number
 *
 * Return: a PTE with the PPN field set to @ppn, and all other fields cleared.
 */
static __always_inline pte_t pte_from_ppn(ppn_t ppn)
{
	info("ppn: 0x%x variable not used\n", ppn);
	return 0UL;
}


/**
 * pte_next_ptb() - Get the next level pagetable from a PTE.
 * @pte: Page table entry.
 *
 * Return: Pointer to the struct pgtable at the address encoded in the PPN field of the PTE.
 */
static __always_inline struct pgtable *pte_next_ptb(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return 0UL;
}

/**
 * ptb_get_ptep() - Get a pointer to the PTE at a given index within a pagetable.
 * @ptb: Pointer to a page table.
 * @index: Index within the page table.
 *
 * Return: Pointer to the PTE entry at index @index within @ptb.
 */
static __always_inline pte_t *ptb_get_ptep(struct pgtable *ptb, u64 index)
{
	return &ptb->entries[index];
}

#define PTE_VALID	0UL
#define PTE_READ	0UL
#define PTE_WRITE	0UL
#define PTE_EXEC	0UL
#define PTE_LEAF	0UL

/* Inline functions to help manipulate PTEs. */

static __always_inline bool pte_valid(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return false;
}

static __always_inline bool pte_readable(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return false;
}

static __always_inline bool pte_writable(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return false;
}

static __always_inline bool pte_executable(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return false;
}

static __always_inline bool pte_leaf(pte_t pte)
{
	info("pte: 0x%x variable not used\n", pte);
	return false;
}

#endif
