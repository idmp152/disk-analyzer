#pragma once
#include "clay.h"

typedef struct {
    Clay_String name;
    Clay_String created_at;
    Clay_String modified_at;
    Clay_String size;
} File;

typedef struct {
    File* files;
    int fileCount;
    int capacity;
} FileList;

typedef struct {
    Clay_String* drives;
    int driveCount;
} DriveList;