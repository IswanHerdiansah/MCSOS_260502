#include <mcsos/kernel/kmem.h>

typedef struct kmem_block {

    size_t size;

    bool free;

    struct kmem_block *next;

} kmem_block_t;

static kmem_block_t *g_head = 0;

void kmem_init(void *buffer, size_t size) {

    g_head = (kmem_block_t *)buffer;

    g_head->size =
        size - sizeof(kmem_block_t);

    g_head->free = true;

    g_head->next = 0;
}

void *kmalloc(size_t size) {

    if (size == 0)
        return 0;

    kmem_block_t *curr = g_head;

    while (curr) {

        if (curr->free &&
            curr->size >= size) {

            size_t remaining =
                curr->size - size;

            if (remaining >
                sizeof(kmem_block_t)) {

                kmem_block_t *new_block =
                    (kmem_block_t *)(
                        (char *)(curr + 1) + size
                    );

                new_block->size =
                    remaining -
                    sizeof(kmem_block_t);

                new_block->free = true;

                new_block->next =
                    curr->next;

                curr->next =
                    new_block;

                curr->size = size;
            }

            curr->free = false;

            return (void *)(curr + 1);
        }

        curr = curr->next;
    }

    return 0;
}

void kfree(void *ptr) {

    if (!ptr)
        return;

    kmem_block_t *block =
        ((kmem_block_t *)ptr) - 1;

    block->free = true;
}

size_t kmem_get_free_bytes(void) {

    size_t total = 0;

    kmem_block_t *curr = g_head;

    while (curr) {

        if (curr->free)
            total += curr->size;

        curr = curr->next;
    }

    return total;
}
