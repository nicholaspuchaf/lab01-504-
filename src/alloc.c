#include <kernel/types.h>
#include <kernel/string.h>
#include <kernel/alloc.h>
#include <arch/pgtable.h>
#include <kernel/panic.h>

struct node {
    struct node *next;
};

static struct node *freelist_head = NULL;
static int is_initialized = 0;

#define PHYSTOP 0x88000000UL

int alloc_init(phys_addr_t start)
{
    phys_addr_t current_page = (phys_addr_t)PPN_UP(start) << PAGE_SHIFT;

    is_initialized = 1;

    for (; current_page < PHYSTOP; current_page += PAGE_SIZE) {
        alloc_free_page(current_page);
    }

    return 0;
}

phys_addr_t alloc_get_page()
{
    if (!is_initialized || freelist_head == NULL) {
        return 0UL;
    }

    struct node *p = freelist_head;
    freelist_head = p->next;

    return (phys_addr_t)p;
}

phys_addr_t alloc_zero_page()
{
    phys_addr_t pa = alloc_get_page();
    
    if (pa != 0UL) {
        memset((void *)pa, 0, PAGE_SIZE);
    }
    
    return pa;
}

int alloc_free_page(phys_addr_t pa)
{
    if (!is_initialized || !IS_ALIGNED(pa)) {
        return -1;
    }

    struct node *p = (struct node *)pa;

    p->next = freelist_head;
    freelist_head = p;

    return 0;
}