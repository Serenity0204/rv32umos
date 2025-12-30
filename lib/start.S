.section .text
.global _start

_start:
    call main
    call exit
    j .