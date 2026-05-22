#ifndef MCSOS_KERNEL_THREAD_H
#define MCSOS_KERNEL_THREAD_H

#include <stddef.h>
#include <stdint.h>

typedef void (*thread_entry_t)(void);

enum thread_state {
    THREAD_READY = 0,
    THREAD_RUNNING = 1
};

struct thread {
    thread_entry_t entry;
    void *stack;
    enum thread_state state;
};

void thread_system_init(void);

struct thread *thread_create(
    thread_entry_t entry
);

void thread_yield(void);

#endif
