void exit(int code)
{
    register int a0 asm("a0") = code;
    register int a7 asm("a7") = 93;
    asm volatile(
        "ecall"
        :
        : "r"(a0), "r"(a7)
        : "memory");
    while (1);
}