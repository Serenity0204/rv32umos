/* linker.ld */
ENTRY(_start)

/* Define segments with specific permissions */
/* FLAGS: 4=Read, 2=Write, 1=Exec */
PHDRS
{
    text PT_LOAD FLAGS(5); /* R + X (4 + 1) */
    data PT_LOAD FLAGS(6); /* R + W (4 + 2) */
}

SECTIONS
{
    /* Start at 0x00000000. */
    . = 0x00000000;

    /* CODE SEGMENT */
    .text : {
        *(.text)        
        *(.text.*)
    } :text

    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } :text

    /* Align to next 4KB Page Boundary */
    /* This ensures Data starts on a new page, separate from Code */
    . = ALIGN(0x1000);

    /* DATA SEGMENT */
    .data : {
        *(.data)
        *(.data.*)
    } :data

    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } :data
}