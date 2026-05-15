cmake_minimum_required(VERSION 3.15)
project(DiskAnalyzer)

set(FLTK_BUILD_TEST OFF CACHE BOOL "Skip FLTK tests")
set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "Skip FLTK examples")
set(FLTK_BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs") 


add_subdirectory(vendor/fltk)

add_executable(disk_analyzer src/main.cpp
                             src/arena.cpp
                             src/ui.cpp
                             src/tree_view.cpp
                             src/file_data_provider.cpp)

target_include_directories(disk_analyzer PRIVATE include)
target_link_libraries(disk_analyzer fltk)

add_compile_options(-Wall -Wextra -Wpedantic)
# if(WIN32)
#     set_target_properties(disk_analyzer PROPERTIES WIN32_EXECUTABLE TRUE)
# endif()
