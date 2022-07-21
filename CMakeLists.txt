cmake_minimum_required(VERSION 3.12)

# Change the project name here!
project(ModSettings LANGUAGES CXX)

# Set this option to ON to be able to use folders / filters in your project when using Visual Studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set the C++ standard to 20 (C++20).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Add RED4ext.SDK as a dependency.

# Uncomment the following line if you want to use the header-only version of RED4ext.SDK.
#option(RED4EXT_HEADER_ONLY "" ON)

add_subdirectory(deps/red4ext.sdk)
set_target_properties(RED4ext.SDK PROPERTIES FOLDER "Dependencies")

# This will mark the following RED4ext variables as advanced. They won't be displayed in CMake's GUI.
mark_as_advanced(
  RED4EXT_BUILD_EXAMPLES
  RED4EXT_HEADER_ONLY
)

# detours

add_library(Detours STATIC)
set_target_properties(Detours PROPERTIES FOLDER "Dependencies")

set(DETOURS_SRC_DIR "${PROJECT_SOURCE_DIR}/deps/detours/src")
file(GLOB_RECURSE HEADER_FILES ${DETOURS_SRC_DIR}/*.h)
file(GLOB_RECURSE SOURCE_FILES ${DETOURS_SRC_DIR}/*.cpp)

# Remove "uimports.cpp" since it throws "detours.h version mismatch" error.
list(REMOVE_ITEM SOURCE_FILES ${DETOURS_SRC_DIR}/uimports.cpp)

target_include_directories(Detours PUBLIC ${DETOURS_SRC_DIR})
target_sources(Detours PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# spdlog

# option(SPDLOG_ENABLE_PCH "" ON)
# option(SPDLOG_DISABLE_DEFAULT_LOGGER "" ON)
# option(SPDLOG_FMT_EXTERNAL "" ON)
# option(SPDLOG_NO_THREAD_ID "" ON)
# option(SPDLOG_WCHAR_FILENAMES "" ON)
# option(SPDLOG_WCHAR_SUPPORT "" ON)

# add_subdirectory(deps/spdlog)
# set_target_properties(spdlog PROPERTIES FOLDER "Dependencies")

# mark_as_advanced(
#   SPDLOG_BUILD_ALL
#   SPDLOG_BUILD_BENCH
#   SPDLOG_BUILD_EXAMPLE
#   SPDLOG_BUILD_EXAMPLE_HO
#   SPDLOG_BUILD_SHARED
#   SPDLOG_BUILD_TESTS
#   SPDLOG_BUILD_TESTS_HO
#   SPDLOG_BUILD_WARNINGS
#   SPDLOG_CLOCK_COARSE
#   SPDLOG_DISABLE_DEFAULT_LOGGER
#   SPDLOG_ENABLE_PCH
#   SPDLOG_FMT_EXTERNAL
#   SPDLOG_FMT_EXTERNAL_HO
#   SPDLOG_INSTALL
#   SPDLOG_NO_ATOMIC_LEVELS
#   SPDLOG_NO_EXCEPTIONS
#   SPDLOG_NO_THREAD_ID
#   SPDLOG_NO_TLS
#   SPDLOG_PREVENT_CHILD_FD
#   SPDLOG_SANITIZE_ADDRESS
#   SPDLOG_TIDY
#   SPDLOG_USE_STD_FORMAT
#   SPDLOG_WCHAR_FILENAMES
#   SPDLOG_WCHAR_SUPPORT
# )

# Include the main project.
add_subdirectory(src/red4ext)