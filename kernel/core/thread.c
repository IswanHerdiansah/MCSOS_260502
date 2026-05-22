#include <stddef.h>
#include <stdint.h>

#include <mcsos/kernel/thread.h>
#include <mcsos/kernel/kmem.h>

#define THREAD_STACK_SIZE 4096u
#define MAX_THREADS 16u

#if !defined(MCSOS_HOST_TEST)

extern void mcsos_context_switch(void);

__attribute__((used))
static void *mcsos_context_switch_ref =
    (void *)&mcsos_context_switch;

#endif

struct thread threads[MAX_THREADS];

static size_t thread_count = 0;

static size_t current_thread = 0;

void thread_system_init(void) {

    thread_count = 0;

    current_thread = 0;
}

struct thread *thread_create(
    thread_entry_t entry
) {

    if (entry == 0) {
        return 0;
    }

    if (thread_count >= MAX_THREADS) {
        return 0;
    }

    struct thread *t =
        &threads[thread_count];

    t->entry = entry;

#if defined(MCSOS_HOST_TEST)

    t->stack = (void *)0x1000;

#else

    t->stack =
        kmalloc(THREAD_STACK_SIZE);

    if (t->stack == 0) {
        return 0;
    }

#endif

    t->state = THREAD_READY;

    ++thread_count;

    return t;
}

void thread_yield(void) {

    if (thread_count == 0) {
        return;
    }

    current_thread++;

    if (current_thread >= thread_count) {
        current_thread = 0;
    }

    struct thread *next =
        &threads[current_thread];

    if (next->entry != 0) {
        next->entry();
    }
}

#if defined(__x86_64__)

__attribute__((noreturn))
void mcsos_thread_trampoline(void) {

    for (;;) {
        __asm__ volatile("hlt");
    }
}

#endif
