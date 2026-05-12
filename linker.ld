ENTRY(_start)

SECTIONS
{
    . = 0xffffffff80000000;

    .text :
    {
        KEEP(*(.text._start))
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }
}
