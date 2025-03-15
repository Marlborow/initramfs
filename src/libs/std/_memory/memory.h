#ifndef MEMORY_H
#define MEMORY_H

#define __MEMORY_POOL_SIZE 65536
#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

extern char __memory_pool[__MEMORY_POOL_SIZE];
extern struct __memory_block* __memory_ptr;

struct __memory_block {
    struct __memory_block* next;
    unsigned long size;
    char isFree;
};

void   __memory_init();
void*  __memory_malloc(unsigned long size);
void   __memory_free(void* block);
void*  __memory_realloc(void* block, unsigned long new_size);

typedef struct __s_memory {
    void* (*allocate)(unsigned long);
    void* (*reallocate)(void*,unsigned long);
    void  (*free)(void* block);
} __s_memory;

#define memory ((struct __s_memory) {__memory_malloc, __memory_realloc, __memory_free})

#endif


