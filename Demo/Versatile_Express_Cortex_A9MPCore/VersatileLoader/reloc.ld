ENTRY(Vector_Init);

MEMORY
{
	ram (rwx) : ORIGIN = 0x0, LENGTH = 64k
}

SECTIONS
{
    .text :
    {
        _text = .;
        __isr_vector_start = .;
        *(.isr_vector)
        __isr_vector_end = .;
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > ram
    .data :
    {
        _data = .;
        *(vtable)
        *(.data*)
        *(.got*)
        _edata = .;
    } > ram
    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > ram
}

