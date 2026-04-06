#include <kernel/printf.h>
#include <kernel/types.h>
#include <arch/csr.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/alloc.h>

void kmain()
{
	info("entered S-mode\n");
	info("loading page tables, bracing for the impact...\n");
	vm_init();
	info("we survived\n");

	info("nothing else to do for now; spinning indefinitely...");
	while (1) {

	}
}
