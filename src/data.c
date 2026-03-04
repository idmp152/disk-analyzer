#include "data.h"
#include "clay.h"
#include "util.h"
#include <stdio.h>
#include <string.h>


static void TreeNode_InitChildren(TreeNode* node) {
    if (!node) return;
    
    node->childCapacity = TREE_INITIAL_CHILD_CAPACITY;
    node->children = (TreeNode**)calloc(node->childCapacity, sizeof(TreeNode*));
    node->childCount = 0;
}

static void TreeNode_EnsureCapacity(TreeNode* node, int minCapacity) {
    if (!node || minCapacity <= node->childCapacity) return;
    
    int newCapacity = node->childCapacity * 3 / 2;
    if (newCapacity < minCapacity) newCapacity = minCapacity;
    
    TreeNode** newChildren = (TreeNode**)realloc(node->children, 
                                                  newCapacity * sizeof(TreeNode*));
    if (newChildren) {
        node->children = newChildren;
        node->childCapacity = newCapacity;
    }
}

static void FileTree_InitVisibleList(FileTree* tree) {
    if (!tree) return;
    
    tree->visibleCapacity = TREE_INITIAL_VISIBLE_CAPACITY;
    tree->visibleNodes = (TreeNode**)calloc(tree->visibleCapacity, sizeof(TreeNode*));
    tree->visibleCount = 0;
}

static void FileTree_EnsureVisibleCapacity(FileTree* tree, int minCapacity) {
    if (!tree || minCapacity <= tree->visibleCapacity) return;
    
    int newCapacity = tree->visibleCapacity * 2;
    if (newCapacity < minCapacity) newCapacity = minCapacity;
    
    TreeNode** newNodes = (TreeNode**)realloc(tree->visibleNodes, 
                                               newCapacity * sizeof(TreeNode*));
    if (newNodes) {
        tree->visibleNodes = newNodes;
        tree->visibleCapacity = newCapacity;
    }
}

void TreeNode_AddChild(TreeNode* parent, TreeNode* child) {
    if (!parent || !child) return;
    
    if (!parent->children) {
        TreeNode_InitChildren(parent);
    }
    
    if (parent->childCount >= parent->childCapacity) {
        TreeNode_EnsureCapacity(parent, parent->childCount + 1);
    }
    
    parent->children[parent->childCount++] = child;
    child->parent = parent;
    child->depth = parent->depth + 1;
}

void TreeNode_ToggleExpand(TreeNode* node) {
    if (!node || node->type == NODE_TYPE_FILE) return;
    node->isExpanded = !node->isExpanded;
}

void TreeNode_CollapseAll(TreeNode* node) {
    if (!node) return;
    
    if (node->type == NODE_TYPE_DIRECTORY) {
        node->isExpanded = false;
    }
    
    for (int i = 0; i < node->childCount; i++) {
        TreeNode_CollapseAll(node->children[i]);
    }
}

void TreeNode_ExpandAll(TreeNode* node) {
    if (!node) return;
    
    if (node->type == NODE_TYPE_DIRECTORY) {
        node->isExpanded = true;
    }
    
    for (int i = 0; i < node->childCount; i++) {
        TreeNode_ExpandAll(node->children[i]);
    }
}

void TreeNode_ExpandToDepth(TreeNode* node, int depth) {
    if (!node) return;
    
    if (node->type == NODE_TYPE_DIRECTORY && node->depth < depth) {
        node->isExpanded = true;
    }
    
    for (int i = 0; i < node->childCount; i++) {
        TreeNode_ExpandToDepth(node->children[i], depth);
    }
}

void FileTree_Init(FileTree* tree) {
    if (!tree) return;
    
    tree->root = NULL;
    FileTree_InitVisibleList(tree);
}

void FileTree_Clear(FileTree* tree) {
    if (!tree) return;
    
    free(tree->visibleNodes);
    tree->root = NULL;
    tree->visibleNodes = NULL;
    tree->visibleCount = 0;
    tree->visibleCapacity = 0;
    
    FileTree_InitVisibleList(tree);
}

void FileTree_BuildVisibleList(FileTree* tree) {
    if (!tree || !tree->root) {
        tree->visibleCount = 0;
        return;
    }
    
    tree->visibleCount = 0;
    
    TreeNode* stack[MAX_TREE_DEPTH];
    int stackTop = 0;
    
    stack[stackTop++] = tree->root;
    
    while (stackTop > 0) {
        TreeNode* node = stack[--stackTop];
        
        if (tree->visibleCount >= tree->visibleCapacity) {
            FileTree_EnsureVisibleCapacity(tree, tree->visibleCount + 64);
        }
        
        if (tree->visibleCount >= tree->visibleCapacity) break;
        
        tree->visibleNodes[tree->visibleCount++] = node;
        
        if (node->isExpanded && node->type == NODE_TYPE_DIRECTORY) {
            for (int i = node->childCount - 1; i >= 0; i--) {
                if (stackTop < MAX_TREE_DEPTH) {
                    stack[stackTop++] = node->children[i];
                }
            }
        }
    }
}

const char* TreeNode_GetTypeString(TreeNode* node) {
    if (!node) return "Unknown";
    return (node->type == NODE_TYPE_DIRECTORY) ? "Directory" : "File";
}

uint64_t TreeNode_CalculateTotalSize(TreeNode* node) {
    if (!node) return 0;
    
    uint64_t total = node->size;
    
    for (int i = 0; i < node->childCount; i++) {
        total += TreeNode_CalculateTotalSize(node->children[i]);
    }
    
    return total;
}

int TreeNode_CountChildren(TreeNode* node, bool recursive) {
    if (!node) return 0;
    
    int count = node->childCount;
    
    if (recursive) {
        for (int i = 0; i < node->childCount; i++) {
            count += TreeNode_CountChildren(node->children[i], true);
        }
    }
    
    return count;
}