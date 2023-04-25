cmake_minimum_required(VERSION 3.24)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/deps/cpcmake")

include(CyberpunkMod)

project(mod_settings VERSION 0.0.8)

configure_mod(
  NAME "Mod Settings"
  SLUG ${PROJECT_NAME}
  AUTHOR "Jack Humbert"
  URL "https://github.com/jackhumbert/mod_settings"
  LICENSE "Licensed under the MIT license. See the license.md in the root project for details."
)

set(ZOLTAN_USER_SIGNATURES "${MOD_RED4EXT_SOURCE_DIR}/Main.cpp")
set(ZOLTAN_ADDRESSES_OUTPUT "${MOD_RED4EXT_SOURCE_DIR}/Addresses.hpp")

if(PROJECT_IS_TOP_LEVEL)
  # find_package(RED4ext.SDK)
  # add_subdirectory(src/red4ext)
  configure_red4ext(src/red4ext)
endif()

configure_archives(src/wolvenkit/packed/mods/ModSettings/archives/ModSettings.archive)
configure_redscript(src/redscript)