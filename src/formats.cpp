#include "formats.hpp"
#include <math.h>
#include <cstring>

const char* available_units[UNIT_SIZE] = {" B", "KB", "MB", "GB", "TB"};

void get_size_string(uint64_t size, char* buffer, size_t buf_size) {
	if (size <= 0) {
		snprintf(buffer, buf_size, "0 B");
		return;
	}

	int unit_idx = (int)(log(size) / log(1024));
	if (unit_idx >= UNIT_SIZE)
		unit_idx = UNIT_SIZE - 1;

	uint64_t divisor = 1ULL << (unit_idx * 10);
	uint64_t whole_part = size / divisor;
	uint64_t remainder = size % divisor;
	double full_size =
	    (double)whole_part + ((double)remainder / divisor);  // all of this trickery to not lose precision
	                                                         // (double only holds 2^53 mantissa)

	snprintf(buffer, buf_size, "%.1f %s", full_size, available_units[unit_idx]);
}

double get_size_percent_string(uint64_t size, uint64_t parent_size, char* buffer, size_t buf_size) {
	double percentage = (double)size / parent_size;
	snprintf(buffer, buf_size, "%.1f %%", percentage * 100);
	return percentage;
}

void get_full_path(FileNode* node, char* buffer, size_t buf_size) {
	if ((buffer == nullptr) || buf_size == 0)
		return;

	buffer[0] = '\0';
	if (node == nullptr)
		return;

	size_t current_pos = buf_size - 1;
	buffer[current_pos] = '\0';

	FileNode* curr = node;
	bool is_first = true;

	while (curr != nullptr) {
		if (curr->name == nullptr) {
			curr = curr->parent;
			continue;
		}

		size_t name_len = strlen(curr->name);

		if (!is_first && curr->name[name_len - 1] != '\\' && curr->name[name_len - 1] != '/') {
			if (current_pos >= 1) {
				current_pos--;
				buffer[current_pos] = '\\';
			}
		}

		if (current_pos >= name_len) {
			current_pos -= name_len;
			memcpy(&buffer[current_pos], curr->name, name_len);
		} else {
			break;
		}

		is_first = false;

		curr = curr->parent;
	}

	if (current_pos > 0 && current_pos < buf_size) {
		size_t string_len = buf_size - current_pos;
		memmove(buffer, &buffer[current_pos], string_len);
	}
}