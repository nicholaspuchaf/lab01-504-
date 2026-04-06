#include <kernel/types.h>
#include <kernel/string.h>
#include <kernel/alloc.h>
#include <arch/pgtable.h>
#include <kernel/panic.h>


int alloc_init(phys_addr_t start)
{
	info("start: 0x%x not used\n", start);
	return -1;
}

phys_addr_t alloc_get_page()
{
	return 0UL;
}

phys_addr_t alloc_zero_page()
{
	return 0UL;
}

int alloc_free_page(phys_addr_t pa)
{
	info("pa: 0x%x not used\n", pa);
	return -1;
}
