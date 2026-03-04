#include "tree_arena.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

void TreeNodeArena_Init(TreeNodeArena* arena) {
    arena->firstChunk = NULL;
    arena->currentChunk = NULL;
    arena->totalAllocated = 0;
    arena->nodeCount = 0;
}

static TreeNodeArenaChunk* TreeNodeArena_CreateChunk(size_t capacity) {
    size_t totalSize = sizeof(TreeNodeArenaChunk) + capacity;
    TreeNodeArenaChunk* chunk = (TreeNodeArenaChunk*)malloc(totalSize);
    
    if (chunk) {
        chunk->next = NULL;
        chunk->used = 0;
        chunk->capacity = capacity;
    }
    
    return chunk;
}

TreeNode* TreeNodeArena_Alloc(TreeNodeArena* arena, const char* name, const char* path, 
                               NodeType type, uint64_t size) {
    size_t nodeSize = sizeof(TreeNode);
    size_t alignedSize = (nodeSize + TREE_ARENA_NODE_ALIGNMENT - 1) & ~(TREE_ARENA_NODE_ALIGNMENT - 1);
    
    if (!arena->currentChunk || 
        arena->currentChunk->used + alignedSize > arena->currentChunk->capacity) {
        
        TreeNodeArenaChunk* newChunk = TreeNodeArena_CreateChunk(TREE_ARENA_CHUNK_SIZE);
        if (!newChunk) return NULL;
        
        if (arena->currentChunk) {
            arena->currentChunk->next = newChunk;
        } else {
            arena->firstChunk = newChunk;
        }
        arena->currentChunk = newChunk;
        arena->totalAllocated += TREE_ARENA_CHUNK_SIZE;
    }
    
    TreeNode* node = (TreeNode*)(arena->currentChunk->data + arena->currentChunk->used);
    arena->currentChunk->used += alignedSize;
    arena->nodeCount++;
    
    memset(node, 0, sizeof(TreeNode));
    
    node->name = ClayStringFromCString(name);
    node->fullPath = ClayStringFromCString(path ? path : "");
    node->size = size;
    node->type = type;
    node->depth = 0;
    node->isExpanded = (type == NODE_TYPE_FILE);
    node->isLoading = false;
    node->parent = NULL;
    node->childCount = 0;
    node->childCapacity = 0;
    node->children = NULL;
    
    if (size >= 1024ULL * 1024 * 1024) {
        snprintf(node->sizeBuffer, sizeof(node->sizeBuffer), "%.2f GB", 
                 size / (1024.0 * 1024 * 1024));
    } else if (size >= 1024 * 1024) {
        snprintf(node->sizeBuffer, sizeof(node->sizeBuffer), "%.2f MB", 
                 size / (1024.0 * 1024));
    } else if (size >= 1024) {
        snprintf(node->sizeBuffer, sizeof(node->sizeBuffer), "%.2f KB", 
                 size / 1024.0);
    } else {
        snprintf(node->sizeBuffer, sizeof(node->sizeBuffer), "%lu B", (unsigned long)size);
    }
    node->displaySize = ClayStringFromCString(node->sizeBuffer);
    
    return node;
}

void TreeNodeArena_Clear(TreeNodeArena* arena) {
    if (!arena) return;
    
    TreeNodeArenaChunk* chunk = arena->firstChunk;
    while (chunk) {
        TreeNodeArenaChunk* next = chunk->next;
        free(chunk);
        chunk = next;
    }
    
    arena->firstChunk = NULL;
    arena->currentChunk = NULL;
    arena->totalAllocated = 0;
    arena->nodeCount = 0;
}

void TreeNodeArena_Destroy(TreeNodeArena* arena) {
    TreeNodeArena_Clear(arena);
}