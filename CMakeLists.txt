cmake_minimum_required(VERSION 3.24)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/cpcmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/deps/red4ext.sdk/cmake")

include(ConfigureVersionFromGit)
configure_version_from_git()

project(mod_settings VERSION ${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH} LANGUAGES NONE)

include(CyberpunkMod)

configure_mod(
  NAME "Mod Settings"
  SLUG ${PROJECT_NAME}
  PREFIX MS
  AUTHOR "Jack Humbert"
  URL "https://github.com/jackhumbert/mod_settings"
  LICENSE "Licensed under the MIT license. See the license.md in the root project for details."
)

set(LOAD_ARCHIVES_FROM_RED4EXT ON)
set(LOAD_REDSCRIPT_FROM_RED4EXT ON)

find_program(ZOLTAN_CLANG_EXE NAMES zoltan-clang.exe PATHS "${MOD_TOOLS_DIR}" CACHE)
find_program(REDSCRIPT_CLI_EXE NAMES redscript-cli.exe PATHS "${MOD_TOOLS_DIR}" CACHE)
find_program(CYBERPUNK_2077_EXE NAMES Cyberpunk2077.exe PATHS "${CYBERPUNK_2077_GAME_DIR}/bin/x64" CACHE DOC "Cyberpunk2077.exe Executable File")

find_package(RedLib)

configure_red4ext(src/red4ext)
configure_red4ext_addresses(Addresses.hpp)

target_include_directories(${MOD_SLUG}.dll
  PUBLIC
    ${MOD_SOURCE_DIR}/include
)

target_link_libraries(${MOD_SLUG}.dll
  PUBLIC
    ArchiveXL
    RedLib
)

target_compile_definitions(${MOD_SLUG}.dll PRIVATE MOD_SETTINGS_DLLDIR_EX)

configure_archives(src/wolvenkit/packed/mods/ModSettings/archives/ModSettings.archive src/archiveXL/ModSettings.archive.xl)

configure_redscript(src/redscript)

configure_folder_file(readme.md)
configure_folder_file(license.md)

# configure_uninstall()
configure_release(${MOD_SLUG}_${MOD_VERSION_STR}.zip)
configure_install()