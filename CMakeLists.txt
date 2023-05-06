cmake_minimum_required(VERSION 3.24)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/deps/cpcmake")

include(CyberpunkMod)

project(mod_settings VERSION 0.1.0)

configure_mod(
  NAME "Mod Settings"
  SLUG ${PROJECT_NAME}
  AUTHOR "Jack Humbert"
  URL "https://github.com/jackhumbert/mod_settings"
  LICENSE "Licensed under the MIT license. See the license.md in the root project for details."
)

find_program(ZOLTAN_CLANG_EXE NAMES zoltan-clang.exe PATHS "${MOD_TOOLS_DIR}" CACHE)
find_program(REDSCRIPT_CLI_EXE NAMES redscript-cli.exe PATHS "${MOD_TOOLS_DIR}" CACHE)
find_program(CYBERPUNK_2077_EXE NAMES Cyberpunk2077.exe PATHS "${CYBERPUNK_2077_GAME_DIR}/bin/x64" CACHE DOC "Cyberpunk2077.exe Executable File")

configure_red4ext(src/red4ext)
configure_red4ext_addresses(Main.cpp Addresses.hpp)
configure_archives(src/wolvenkit/packed/mods/ModSettings/archives/ModSettings.archive src/archiveXL/ModSettings.archive.xl)
configure_redscript(src/redscript)
configure_folder_file(readme.md)
configure_folder_file(license.md)
configure_uninstall()
configure_install()