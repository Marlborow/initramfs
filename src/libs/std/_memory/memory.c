#include "memory.h"

char __memory_pool[__MEMORY_POOL_SIZE];
struct __memory_block* __memory_ptr = ((struct __memory_block*)0);

void __memory_init()
{
    __memory_ptr = (struct __memory_block*)__memory_pool;
    __memory_ptr->next = (struct __memory_block*)0;
    __memory_ptr->isFree = 1;
    __memory_ptr->size = __MEMORY_POOL_SIZE - sizeof(struct __memory_block);
}

void* __memory_malloc(unsigned long size)
{
    if (__memory_ptr == ((struct __memory_block*)0)) {
        __memory_init();
    }

    size = ALIGN(size);
    struct __memory_block* current = __memory_ptr;

    while (current) {
        if (current->isFree && current->size >= size) {
            if (current->size > size + sizeof(struct __memory_block)) {
                struct __memory_block* newBlock = (struct __memory_block*)
                    ((char*)current + sizeof(struct __memory_block) + size);
                newBlock->next = current->next;
                newBlock->isFree = 1;
                newBlock->size = current->size - size - sizeof(struct __memory_block);
                current->next = newBlock;
            }

            current->isFree = 0;
            current->size = size;
            return (char*)current + sizeof(struct __memory_block);
        }
        current = current->next;
    }
    return ((void*)0);
}

void __memory_free(void* block)
{
    if (!block) return;

    struct __memory_block* memoryBlock =
        (struct __memory_block*)((char*)block - sizeof(struct __memory_block));
    memoryBlock->isFree = 1;

    struct __memory_block* current = __memory_ptr;
    while (current) {
        if (current->isFree && current->next && current->next->isFree) {
            current->size += sizeof(struct __memory_block) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void* __memory_realloc(void* block, unsigned long new_size)
{
    if (!block) return __memory_malloc(new_size);
    if (new_size == 0) { __memory_free(block); return ((void*)0); }

    new_size = ALIGN(new_size);
    struct __memory_block* memoryBlock =
        (struct __memory_block*)((char*)block - sizeof(struct __memory_block));

    if (memoryBlock->size >= new_size) {
        return block;
    }

    if (memoryBlock->next && memoryBlock->next->isFree &&
        memoryBlock->size + sizeof(struct __memory_block) + memoryBlock->next->size >= new_size)
    {
        struct __memory_block* nextBlock = memoryBlock->next;
        memoryBlock->size += sizeof(struct __memory_block) + nextBlock->size;
        memoryBlock->next = nextBlock->next;
        return block;
    }

    void* newBlock = __memory_malloc(new_size);
    if (newBlock) {
        char* src = (char*)block;
        char* dest = (char*)newBlock;
        for (unsigned long i = 0; i < memoryBlock->size; i++) {
            dest[i] = src[i];
        }
        __memory_free(block);
    }
    return newBlock;
}


