/* linker.ld */
ENTRY(_start)

SECTIONS
{
    /* 設定起始位置為 0x80000000 */
    . = 0x80000000;

    /* 1. 程式碼段 (.text) */
    .text : {
        *(.text)           /* 所有的 code */
        *(.text.*)
    }

    /* 2. 唯讀資料段 (.rodata) - 例如字串常量 */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    /* 3. 可讀寫資料段 (.data) - 例如已初始化的全域變數 */
    .data : {
        *(.data)
        *(.data.*)
    }

    /* 4. 未初始化資料段 (.bss) - 例如 memory_pool */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
}