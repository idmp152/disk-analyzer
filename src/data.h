#pragma once
#include "clay.h"

#define MAX_TREE_DEPTH 64
#define TREE_INITIAL_CHILD_CAPACITY 16
#define TREE_INITIAL_VISIBLE_CAPACITY 256


typedef struct {
    Clay_String* drives;
    int driveCount;
} DriveList;

typedef enum {
    NODE_TYPE_FILE,
    NODE_TYPE_DIRECTORY
} NodeType;

typedef struct TreeNode {
    Clay_String name;
    Clay_String fullPath;
    uint64_t size;
    NodeType type;
    
    struct TreeNode* parent;
    struct TreeNode** children;
    int childCount;
    int childCapacity;
    
    int depth;
    bool isExpanded;
    bool isLoading;
    
    Clay_String displaySize;
    char sizeBuffer[32];
} TreeNode;

typedef struct {
    TreeNode* root;
    
    TreeNode** visibleNodes;
    int visibleCount;
    int visibleCapacity;
} FileTree;


void TreeNode_AddChild(TreeNode* parent, TreeNode* child);
void TreeNode_ToggleExpand(TreeNode* node);
void TreeNode_CollapseAll(TreeNode* node);
void TreeNode_ExpandAll(TreeNode* node);
void TreeNode_ExpandToDepth(TreeNode* node, int depth);

void FileTree_Init(FileTree* tree);
void FileTree_Clear(FileTree* tree);
void FileTree_BuildVisibleList(FileTree* tree);

const char* TreeNode_GetTypeString(TreeNode* node);
uint64_t TreeNode_CalculateTotalSize(TreeNode* node);
int TreeNode_CountChildren(TreeNode* node, bool recursive);