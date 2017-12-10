/*
 * Copyright (c) 2017, HongluoStuido.com. All rights reserved.
 */

#include <platform_def.h>

OUTPUT_FORMAT(PLATFORM_LINKER_FORMAT)
OUTPUT_ARCH(PLATFORM_LINKER_ARCH)
ENTRY(hyp_entrypoint)

MEMORY {
    RAM (rwx): ORIGIN = HYP_BASE, LENGTH = HYP_LIMIT - HYP_BASE
}

SECTIONS
{
    . = HYP_BASE;
    ASSERT(. == ALIGN(4096),
           "BL31_BASE address is not aligned on a page boundary.")

    ro . : {
        __RO_START__ = .;
        *raspihyp.o(.text*)
        *(.text*)
        *(.rodata*)

        *(.vectors)
        __RO_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked as read-only,
         * executable.  No RW data from the next section must creep in.
         * Ensure the rest of the current memory page is unused.
         */
        . = NEXT(4096);
        __RO_END__ = .;
    } >RAM

    stacks (NOLOAD) : {
        __STACKS_START__ = .;
        *(hyp_normal_stacks)
        __STACKS_END__ = .;
    } >RAM

    .data . : {
        __DATA_START__ = .;
        *(.data*)
        __DATA_END__ = .;
    } >RAM

#ifdef HYP_PROGBITS_LIMIT
    ASSERT(. <= HYP_PROGBITS_LIMIT, "BL3-1 progbits has exceeded its limit.")
#endif

    /*
     * The .bss section gets initialised to 0 at runtime.
     * Its base address must be 16-byte aligned.
     */
    .bss : ALIGN(16) {
        __BSS_START__ = .;
        *(.bss*)
        *(COMMON)
        __BSS_END__ = .;
    } >RAM

    __HYP_END__ = .;

    __BSS_SIZE__ = SIZEOF(.bss);

    ASSERT(. <= HYP_LIMIT, "BL3-1 image has exceeded its limit.")
}
