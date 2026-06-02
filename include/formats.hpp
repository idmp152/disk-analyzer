#pragma once
#include <cstdint>
#include "file_data_provider.hpp"

#define UNIT_SIZE 5

void get_size_string(uint64_t size, char* buffer, size_t buf_size);
double get_size_percent_string(uint64_t size, uint64_t parent_size, char* buffer, size_t buf_size);
void get_full_path(FileNode* node, char* buffer, size_t buf_size);