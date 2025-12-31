#ifndef SYSDEF_H
#define SYSDEF_H

#define _syscall0(id)                                          \
    ({                                                         \
        register long a7 asm("a7") = (long)(id);               \
        register long a0 asm("a0");                            \
        asm volatile("ecall" : "=r"(a0) : "r"(a7) : "memory"); \
        a0;                                                    \
    })

#define _syscall1(id, a)                                        \
    ({                                                          \
        register long a7 asm("a7") = (long)(id);                \
        register long _a0 asm("a0") = (long)(a);                \
        asm volatile("ecall" : "+r"(_a0) : "r"(a7) : "memory"); \
        _a0;                                                    \
    })

#define _syscall2(id, a, b)                                               \
    ({                                                                    \
        register long a7 asm("a7") = (long)(id);                          \
        register long _a0 asm("a0") = (long)(a);                          \
        register long _a1 asm("a1") = (long)(b);                          \
        asm volatile("ecall" : "+r"(_a0) : "r"(a7), "r"(_a1) : "memory"); \
        _a0;                                                              \
    })

#define _syscall3(id, a, b, c)                                                      \
    ({                                                                              \
        register long a7 asm("a7") = (long)(id);                                    \
        register long _a0 asm("a0") = (long)(a);                                    \
        register long _a1 asm("a1") = (long)(b);                                    \
        register long _a2 asm("a2") = (long)(c);                                    \
        asm volatile("ecall" : "+r"(_a0) : "r"(a7), "r"(_a1), "r"(_a2) : "memory"); \
        _a0;                                                                        \
    })

#define _syscall4(id, a, b, c, d)                                                             \
    ({                                                                                        \
        register long a7 asm("a7") = (long)(id);                                              \
        register long _a0 asm("a0") = (long)(a);                                              \
        register long _a1 asm("a1") = (long)(b);                                              \
        register long _a2 asm("a2") = (long)(c);                                              \
        register long _a3 asm("a3") = (long)(d);                                              \
        asm volatile("ecall" : "+r"(_a0) : "r"(a7), "r"(_a1), "r"(_a2), "r"(_a3) : "memory"); \
        _a0;                                                                                  \
    })

#define _syscall5(id, a, b, c, d, e)                                                                    \
    ({                                                                                                  \
        register long a7 asm("a7") = (long)(id);                                                        \
        register long _a0 asm("a0") = (long)(a);                                                        \
        register long _a1 asm("a1") = (long)(b);                                                        \
        register long _a2 asm("a2") = (long)(c);                                                        \
        register long _a3 asm("a3") = (long)(d);                                                        \
        register long _a4 asm("a4") = (long)(e);                                                        \
        asm volatile("ecall" : "+r"(_a0) : "r"(a7), "r"(_a1), "r"(_a2), "r"(_a3), "r"(_a4) : "memory"); \
        _a0;                                                                                            \
    })

#define _syscall6(id, a, b, c, d, e, f)                                                                           \
    ({                                                                                                            \
        register long a7 asm("a7") = (long)(id);                                                                  \
        register long _a0 asm("a0") = (long)(a);                                                                  \
        register long _a1 asm("a1") = (long)(b);                                                                  \
        register long _a2 asm("a2") = (long)(c);                                                                  \
        register long _a3 asm("a3") = (long)(d);                                                                  \
        register long _a4 asm("a4") = (long)(e);                                                                  \
        register long _a5 asm("a5") = (long)(f);                                                                  \
        asm volatile("ecall" : "+r"(_a0) : "r"(a7), "r"(_a1), "r"(_a2), "r"(_a3), "r"(_a4), "r"(_a5) : "memory"); \
        _a0;                                                                                                      \
    })

// This selects the correct macro based on the number of arguments provided.
#define _GET_MACRO(_0, _1, _2, _3, _4, _5, _6, NAME, ...) NAME

#define syscall(...) _GET_MACRO(__VA_ARGS__, _syscall6, _syscall5, _syscall4, _syscall3, _syscall2, _syscall1, _syscall0)(__VA_ARGS__)

#endif