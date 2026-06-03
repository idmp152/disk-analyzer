#include "file_data_provider.hpp"
#include <FL/Fl.H>
#include <FL/fl_utf8.h>
#include <windows.h>
#include <iostream>
#include "formats.hpp"

uint64_t* is_directory_mask;
uint64_t* is_expanded_mask;

FileNode* file_tree_buffer;
size_t file_tree_buffer_size = 0;

Arena* string_arena;
std::vector<DriveInfo> drives;

void provider_init(Arena* str_arena,
                   FileNode* file_buffer,
                   uint64_t* dir_mask,
                   uint64_t* exp_mask) {
  is_directory_mask = dir_mask;
  is_expanded_mask = exp_mask;
  file_tree_buffer = file_buffer;
  string_arena = str_arena;
}

char* utf16_to_utf8(const wchar_t* str) {
  int size_needed =
      WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
  char* new_str = (char*)arena_alloc_align(
      string_arena, size_needed + 1,
      DEFAULT_ALIGNMENT);  // TODO(IlyaBelykh): handle out of memory possible
                           // exceptions
  WideCharToMultiByte(CP_UTF8, 0, str, -1, new_str, size_needed, NULL, NULL);
  return new_str;
}

void iterate_dir(const char* path, FileNode* parent, ScanContext* ctx) {
  if (ctx->should_cancel.load())
    return;

  std::string search_mask = path;
  search_mask += "\\*";

  wchar_t w_search_mask[MAX_PATH];
  fl_utf8towc(search_mask.c_str(), search_mask.size(), w_search_mask, MAX_PATH);

  WIN32_FIND_DATAW data;
  HANDLE hFind = FindFirstFileW(w_search_mask, &data);

  FileNode dummy_node;
  dummy_node.next_sibling = nullptr;
  FileNode* prev_node = &dummy_node;
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      if (ctx->should_cancel.load())
        break;

      if (wcscmp(data.cFileName, L".") == 0 ||
          wcscmp(data.cFileName, L"..") == 0) {
        continue;
      }

      char* filename = utf16_to_utf8(data.cFileName);

      FileNode* file =
          &file_tree_buffer[file_tree_buffer_size++];  // TODO(IlyaBelykh):
                                                       // handle out of memory
                                                       // possible exceptions
      file->name = filename;
      file->parent = parent;
      prev_node->next_sibling = file;
      prev_node = file;
      ctx->files_scanned++;

      if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        search_mask = path;
        search_mask += "\\";
        search_mask += filename;
        set_bit(is_directory_mask, (uint64_t)(file - file_tree_buffer));
        iterate_dir(search_mask.c_str(), file, ctx);
      } else {
        file->size = ((uint64_t)(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
      }

      parent->size += file->size;

      if (ctx->files_scanned % 500 == 0) {
        Fl::awake();
      }
    } while (FindNextFileW(hFind, &data));
    FindClose(hFind);
    parent->first_child = dummy_node.next_sibling;
  }
}

FileNode* add_root_node(const char* path) {
  FileNode* root = &file_tree_buffer[file_tree_buffer_size++];
  set_bit(is_directory_mask, (uint64_t)(root - file_tree_buffer));
  set_bit(is_expanded_mask, (uint64_t)(root - file_tree_buffer));
  root->name = path;
  return root;
}

void traverse_tree_out(FileNode* root,
                       unsigned short depth,
                       std::ostream& stream) {
  FileNode* curr = root;
  while (curr) {
    char size_buf[32];
    char percent_buf[32];
    char format_buf[128];
    get_size_string(curr->size, size_buf, 32);
    get_size_percent_string(curr->size,
                            (curr->parent) ? curr->parent->size : curr->size,
                            percent_buf, 32);
    snprintf(format_buf, 128, " (%s) [%s]", size_buf, percent_buf);
    std::string padding(depth * 2, ' ');
    stream << padding
           << (get_bit(is_directory_mask, (uint64_t)(curr - file_tree_buffer))
                   ? "- "
                   : "> ")
           << curr->name << format_buf << std::endl;
    traverse_tree_out(curr->first_child, depth + 1, stream);
    curr = curr->next_sibling;
  }
}

void traverse_tree_csv(FileNode* root,
                       unsigned short depth,
                       std::ostream& stream) {
  FileNode* curr = root;
  while (curr) {
    uint64_t node_index = (uint64_t)(curr - file_tree_buffer);
    bool is_dir = get_bit(is_directory_mask, node_index);

    for (int i = 0; i < depth; ++i) {
      stream << ";";
    }
    stream << curr->name;

    const int MAX_EXPECTED_DEPTH = 12;
    for (int i = depth; i < MAX_EXPECTED_DEPTH; ++i) {
      stream << ";";
    }

    char size_buf[32];
    get_size_string(curr->size, size_buf, 32);
    stream << (is_dir ? "Directory" : "File") << ";";
    stream << curr->size << ";";
    stream << size_buf << ";";

    char percent_buf[32];
    get_size_percent_string(curr->size,
                            (curr->parent) ? curr->parent->size : curr->size,
                            percent_buf, 32);
    stream << percent_buf << std::endl;

    traverse_tree_csv(curr->first_child, depth + 1, stream);

    curr = curr->next_sibling;
  }
}

void fill_drive_info() {
  WCHAR buffer[254];
  DWORD size = GetLogicalDriveStringsW(254, buffer);

  if (size == 0 || size > 254)
    return;
  WCHAR* drive_letter = buffer;

  while (*drive_letter != L'\0') {
    UINT type = GetDriveTypeW(drive_letter);

    if (type == DRIVE_UNKNOWN || type == DRIVE_NO_ROOT_DIR ||
        type == DRIVE_CDROM) {
      drive_letter += wcslen(drive_letter) + 1;
      continue;
    }

    DriveInfo info;
    char name_copy_buf[256];
    fl_utf8fromwc(name_copy_buf, 256, drive_letter, wcslen(drive_letter) + 1);
    info.name =
        name_copy_buf;  // TODO(IlyaBelykh): Look at this, since utf16_to_utf8
                        // stores the new string into the string arena
    info.is_ntfs = false;

    info.total_size = 0;
    info.free_size = 0;

    WCHAR fs_name[MAX_PATH] = L"";

    if (GetVolumeInformationW(drive_letter, nullptr, 0, nullptr, nullptr,
                              nullptr, fs_name, MAX_PATH)) {
      info.is_ntfs = (wcscmp(fs_name, L"NTFS") == 0);
    }

    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes,
        totalNumberOfFreeBytes;

    if (GetDiskFreeSpaceExW(drive_letter, &freeBytesAvailable,
                            &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
      info.total_size = totalNumberOfBytes.QuadPart;
      info.free_size = totalNumberOfFreeBytes.QuadPart;
    }

    drives.push_back(info);
    drive_letter += wcslen(drive_letter) + 1;
  }
}

FileNode* merge_sibling_lists(FileNode* a, FileNode* b) {
  if (!a)
    return b;
  if (!b)
    return a;

  FileNode dummy;
  FileNode* tail = &dummy;

  while (a && b) {
    if (a->size >= b->size) {
      tail->next_sibling = a;
      a = a->next_sibling;
    } else {
      tail->next_sibling = b;
      b = b->next_sibling;
    }
    tail = tail->next_sibling;
  }

  if (a) {
    tail->next_sibling = a;
  } else {
    tail->next_sibling = b;
  }

  return dummy.next_sibling;
}

FileNode* merge_sort_siblings(FileNode* head) {
  if (!head || !head->next_sibling) {
    return head;
  }

  FileNode* slow = head;
  FileNode* fast = head->next_sibling;

  while (fast && fast->next_sibling) {
    slow = slow->next_sibling;
    fast = fast->next_sibling->next_sibling;
  }

  FileNode* mid = slow->next_sibling;
  slow->next_sibling = nullptr;

  FileNode* left = merge_sort_siblings(head);
  FileNode* right = merge_sort_siblings(mid);

  return merge_sibling_lists(left, right);
}

void sort_directory_tree(FileNode* root) {
  if (!root)
    return;

  if (root->first_child) {
    root->first_child = merge_sort_siblings(root->first_child);
  }

  FileNode* curr = root->first_child;
  while (curr) {
    sort_directory_tree(curr);
    curr = curr->next_sibling;
  }
}
