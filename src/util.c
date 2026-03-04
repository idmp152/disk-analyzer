#include "clay.h"
#include <string.h>

Clay_String ClayStringFromCString(const char* str) {
    return (Clay_String) {
        .chars = str,
        .length = (int)strlen(str),
        .isStaticallyAllocated = false
    };
}