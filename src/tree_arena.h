#pragma once
#include "data.h"
#include <stdint.h>
#include <stddef.h>

#define TREE_ARENA_CHUNK_SIZE (1024 * 1024)
#define TREE_ARENA_NODE_ALIGNMENT 8

typedef struct TreeNodeArenaChunk {
    struct TreeNodeArenaChunk* next;
    size_t used;
    size_t capacity;
    char data[];
} TreeNodeArenaChunk;

typedef struct {
    TreeNodeArenaChunk* firstChunk;
    TreeNodeArenaChunk* currentChunk;
    size_t totalAllocated;
    size_t nodeCount;
} TreeNodeArena;

void TreeNodeArena_Init(TreeNodeArena* arena);
void TreeNodeArena_Destroy(TreeNodeArena* arena);
TreeNode* TreeNodeArena_Alloc(TreeNodeArena* arena, const char* name, const char* path, 
                               NodeType type, uint64_t size);
void TreeNodeArena_Clear(TreeNodeArena* arena);