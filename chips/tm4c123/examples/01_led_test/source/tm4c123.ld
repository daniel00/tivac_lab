/* tm4c123.ld */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.vectors)) /* startup.c에 쓴 벡터 테이블을 맨 앞에 유지해라! */
        *(.text*)         /* 나머지 모든 코드들 */
        *(.rodata*)       /* 읽기 전용 데이터 */
    } > FLASH

    .data :
    {
        *(.data*)         /* 초기화된 변수들 */
    } > SRAM AT > FLASH

    .bss :
    {
        *(.bss*)          /* 초기화되지 않은 변수들 */
    } > SRAM
}
