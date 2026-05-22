#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <mcsos/kernel/kmem.h>

static uint8_t heap[4096];

int main(void) {

    kmem_init(heap, sizeof(heap));

    void *a = kmalloc(64);

    assert(a != 0);

    void *b = kmalloc(128);

    assert(b != 0);

    kfree(a);

    size_t free_bytes =
        kmem_get_free_bytes();

    assert(free_bytes > 0);

    puts("M8 KMEM host unit test: PASS");

    return 0;
}
