#include "file_provider.h"
#include <string.h>
#include <time.h>

static TreeNode* AddNode(TreeNodeArena* arena, TreeNode* parent,
                         const char* name, const char* path, 
                         NodeType type, uint64_t size) {
    TreeNode* node = TreeNodeArena_Alloc(arena, name, path, type, size);
    if (node && parent) {
        TreeNode_AddChild(parent, node);
    }
    return node;
}

void FileProvider_FillTree(FileTree* tree, TreeNodeArena* arena, const char* rootPath) {
    if (!tree || !arena) return;
    
    FileTree_Clear(tree);
    
    TreeNode* root = AddNode(arena, NULL, "C:", "C:\\", NODE_TYPE_DIRECTORY, 
                             500ULL * 1024 * 1024 * 1024);
    if (!root) return;
    
    root->isExpanded = true;
    tree->root = root;
    
    TreeNode* users = AddNode(arena, root, "Users", "C:\\Users", NODE_TYPE_DIRECTORY,
                              150ULL * 1024 * 1024 * 1024);
    TreeNode* programs = AddNode(arena, root, "Program Files", "C:\\Program Files", 
                                 NODE_TYPE_DIRECTORY, 250ULL * 1024 * 1024 * 1024);
    TreeNode* windows = AddNode(arena, root, "Windows", "C:\\Windows", NODE_TYPE_DIRECTORY,
                                100ULL * 1024 * 1024 * 1024);
    
    TreeNode* admin = AddNode(arena, users, "Admin", "C:\\Users\\Admin", NODE_TYPE_DIRECTORY,
                              50ULL * 1024 * 1024 * 1024);
    
    AddNode(arena, admin, "Desktop", "C:\\Users\\Admin\\Desktop", NODE_TYPE_DIRECTORY,
            2ULL * 1024 * 1024 * 1024);
    AddNode(arena, admin, "Documents", "C:\\Users\\Admin\\Documents", NODE_TYPE_DIRECTORY,
            15ULL * 1024 * 1024 * 1024);
    AddNode(arena, admin, "Downloads", "C:\\Users\\Admin\\Downloads", NODE_TYPE_DIRECTORY,
            30ULL * 1024 * 1024 * 1024);
    
    AddNode(arena, admin, "report.pdf", "C:\\Users\\Admin\\Documents\\report.pdf", 
            NODE_TYPE_FILE, 2 * 1024 * 1024);
    AddNode(arena, admin, "budget.xlsx", "C:\\Users\\Admin\\Documents\\budget.xlsx", 
            NODE_TYPE_FILE, 5 * 1024 * 1024);
    AddNode(arena, admin, "notes.txt", "C:\\Users\\Admin\\Documents\\notes.txt", 
            NODE_TYPE_FILE, 50 * 1024);
    
    AddNode(arena, admin, "installer.exe", "C:\\Users\\Admin\\Downloads\\installer.exe", 
            NODE_TYPE_FILE, 150 * 1024 * 1024);
    AddNode(arena, admin, "video.mp4", "C:\\Users\\Admin\\Downloads\\video.mp4", 
            NODE_TYPE_FILE, 1500 * 1024 * 1024);
    
    AddNode(arena, programs, "MyApp", "C:\\Program Files\\MyApp", NODE_TYPE_DIRECTORY,
            500 * 1024 * 1024);
    AddNode(arena, programs, "Common Files", "C:\\Program Files\\Common Files", 
            NODE_TYPE_DIRECTORY, 10ULL * 1024 * 1024 * 1024);
    
    TreeNode_ExpandToDepth(root, 1);
}