.text
.global context_switch

# void context_switch(void** old_sp, void* new_sp);
# %rdi = old_sp (pointer to Thread A's saved stack pointer variable)
# %rsi = new_sp (Thread B's saved stack pointer)

context_switch:
    # save callee-saved registers of the current thread
    pushq %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    # save current %rsp into *old_sp
    movq %rsp, (%rdi)

    # switch to new thread's stack
    movq %rsi, %rsp

    # restore callee-saved registers of the new thread
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp

    # return to new thread's saved RIP
    ret

.section .note.GNU-stack, "", @progbits