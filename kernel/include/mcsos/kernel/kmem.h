#ifndef MCSOS_KERNEL_KMEM_H
#define MCSOS_KERNEL_KMEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void kmem_init(void *buffer, size_t size);

void *kmalloc(size_t size);

void kfree(void *ptr);

size_t kmem_get_free_bytes(void);

#endif
