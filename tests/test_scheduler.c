#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <mcsos/kernel/kmem.h>
#include <mcsos/kernel/thread.h>

#define TEST_HEAP_SIZE (64u * 1024u)

static uint8_t test_heap[TEST_HEAP_SIZE];

static volatile uint64_t g_counter_a = 0;
static volatile uint64_t g_counter_b = 0;

static void thread_a(void) {
    g_counter_a++;
}

static void thread_b(void) {
    g_counter_b++;
}

int main(void) {

    kmem_init(
        test_heap,
        sizeof(test_heap)
    );

    thread_system_init();

    struct thread *ta =
        thread_create(thread_a);

    assert(ta != 0);

    struct thread *tb =
        thread_create(thread_b);

    assert(tb != 0);

    assert(ta != tb);

    thread_yield();

    printf("[M9] host test PASS\n");

    return 0;
}
