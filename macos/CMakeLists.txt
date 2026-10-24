cmake_minimum_required(VERSION 3.21)
# macOS (Apple Silicon, arm64) build of the Mod Settings RED4ext plugin. R41c, local only.
#
# Pattern: cp2077-codeware/macos/CMakeLists.txt (a generated copy of the Mac SDK Relocation-inl.hpp with this plugin's rows
# injected, an unmapped hash that traps instead of resolving to 0) and MacInputFix/ (a small standalone plugin build).
# Windows code paths of src/red4ext are unchanged: every Mac edit there is under __APPLE__, the rest lives in this folder.
#
#   cmake -S macos -B macos/build [-DRED4EXT_SDK_DIR=../../RED4ext.SDK-macos] [-DMODSETTINGS_MAC_APPLYOVERRIDES=OFF]
#   cmake --build macos/build -j
#
# The version is the git tag (v0.2.21 -> 0.2.21), the same string upstream's ConfigureVersionFromGit gives a tagged build, without
# the commit metadata, so the Mac build reports "Starting up Mod Settings 0.2.21" like the release DLL.

set(MS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
find_package(Git QUIET)
set(_ms_tag "")
if(GIT_FOUND)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${MS_ROOT}" describe --tags --abbrev=0 --match "v[0-9]*"
                    OUTPUT_VARIABLE _ms_tag OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
endif()
string(REGEX MATCH "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ms_ver "${_ms_tag}")
if(_ms_ver)
    set(MS_VER_MAJOR ${CMAKE_MATCH_1})
    set(MS_VER_MINOR ${CMAKE_MATCH_2})
    set(MS_VER_PATCH ${CMAKE_MATCH_3})
else()
    message(WARNING "no v<major>.<minor>.<patch> tag found, using 0.2.21")
    set(MS_VER_MAJOR 0)
    set(MS_VER_MINOR 2)
    set(MS_VER_PATCH 21)
endif()
project(mod_settings VERSION ${MS_VER_MAJOR}.${MS_VER_MINOR}.${MS_VER_PATCH} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

if(NOT APPLE)
    message(FATAL_ERROR "This target builds the macOS Mod Settings dylib")
endif()

option(MODSETTINGS_MAC_APPLYOVERRIDES
    "Attach the ApplyOverrides hook on the body 0x30A0D5C (R41b: a real entry, void(UserMapping*)). OFF compiles it out; then also take 0x30a0d5c out of hook-offsets.txt."
    ON)

# Game-side symbols (the game's own functions) are bound by address at run time, not by name; the dylib has no unresolved C++ symbols
# of its own except what check-undefined.sh allows.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -undefined dynamic_lookup")

set(RED4EXT_SDK_DIR "${MS_ROOT}/../../RED4ext.SDK-macos" CACHE PATH "macOS RED4ext SDK checkout")
get_filename_component(RED4EXT_SDK_DIR "${RED4EXT_SDK_DIR}" ABSOLUTE)
set(SDK_INCLUDE "${RED4EXT_SDK_DIR}/include")
if(NOT EXISTS "${SDK_INCLUDE}/RED4ext/Relocation-inl.hpp")
    message(FATAL_ERROR "no RED4ext SDK headers at ${SDK_INCLUDE} (set -DRED4EXT_SDK_DIR=...)")
endif()

# Reproducible: no checkout path ends up in the dylib.
get_filename_component(_ms_root_abs "${MS_ROOT}" ABSOLUTE)
add_compile_options("-ffile-prefix-map=${_ms_root_abs}=mod_settings" "-ffile-prefix-map=${RED4EXT_SDK_DIR}=RED4ext.SDK-macos"
                    "-ffile-prefix-map=${CMAKE_BINARY_DIR}=build")

# ---------------------------------------------------------------------------------------------------------------------------------
# The Mac SDK address table (RED4ext/Relocation-inl.hpp) with this plugin's rows injected, and a trap for an unmapped hash.
# Same two edits as Codeware's R16a / R17, written to a generated copy that comes first on the include path. The SDK is not touched.
set(SDK_RELOC "${SDK_INCLUDE}/RED4ext/Relocation-inl.hpp")
set(SDK_ROWS_INC "${CMAKE_CURRENT_SOURCE_DIR}/ModSettingsSdkRows.inc")
set(SDK_OVERLAY "${CMAKE_CURRENT_BINARY_DIR}/sdk-overlay")
file(READ "${SDK_RELOC}" SDK_RELOC_TEXT)
file(READ "${SDK_ROWS_INC}" SDK_ROWS_TEXT)
set(SDK_ROWS_MARKER "        { 4201325776, 0x109BAA8 },  // CGlobalFunction_ctor\n")
string(FIND "${SDK_RELOC_TEXT}" "${SDK_ROWS_MARKER}" SDK_ROWS_POS)
if(SDK_ROWS_POS EQUAL -1)
    message(FATAL_ERROR "the anchor row (CGlobalFunction_ctor) is not in ${SDK_RELOC}. Update the anchor in CMakeLists.txt.")
endif()
string(REPLACE "${SDK_ROWS_MARKER}" "${SDK_ROWS_MARKER}${SDK_ROWS_TEXT}" SDK_RELOC_TEXT "${SDK_RELOC_TEXT}")

# An unmapped hash must never resolve to 0 (the caller would jump to null). Replace the unknown-hash tail of the Mac Resolve with a
# call into macos/MacTrap.cpp: the returned stub logs once and returns zero.
set(SDK_TAIL_START "    // Address not found or placeholder - return a non-zero sentinel value")
set(SDK_TAIL_END "    return 0;\n#else\n    if constexpr (Detail::AddressResolverOverride<uint32_t>::value)")
string(FIND "${SDK_RELOC_TEXT}" "${SDK_TAIL_START}" SDK_TAIL_A)
string(FIND "${SDK_RELOC_TEXT}" "${SDK_TAIL_END}" SDK_TAIL_B)
if(SDK_TAIL_A EQUAL -1 OR SDK_TAIL_B EQUAL -1)
    message(FATAL_ERROR "the unknown-hash tail of Resolve was not found in ${SDK_RELOC}. Update the anchors in CMakeLists.txt.")
endif()
string(SUBSTRING "${SDK_RELOC_TEXT}" 0 ${SDK_TAIL_A} SDK_HEAD)
math(EXPR SDK_REST_FROM "${SDK_TAIL_B} + 14")
string(SUBSTRING "${SDK_RELOC_TEXT}" ${SDK_REST_FROM} -1 SDK_REST)
set(SDK_RELOC_TEXT "${SDK_HEAD}    // Mod Settings trap: logs on the first call of the returned stub, then returns 0. Never returns 0 itself.\n    return modsettings_mac_trap_for_hash(aHash);\n${SDK_REST}")
set(SDK_DECL_MARKER "extern \"C\" uintptr_t tweakxl_class_vslot(uint32_t aByteOffset);\n")
string(FIND "${SDK_RELOC_TEXT}" "${SDK_DECL_MARKER}" SDK_DECL_POS)
if(SDK_DECL_POS EQUAL -1)
    message(FATAL_ERROR "the extern declaration anchor was not found in ${SDK_RELOC}.")
endif()
string(REPLACE "${SDK_DECL_MARKER}" "${SDK_DECL_MARKER}extern \"C\" uintptr_t modsettings_mac_trap_for_hash(uint32_t aHash);\n" SDK_RELOC_TEXT "${SDK_RELOC_TEXT}")
file(WRITE "${SDK_OVERLAY}/RED4ext/Relocation-inl.hpp" "${SDK_RELOC_TEXT}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${SDK_RELOC}" "${SDK_ROWS_INC}")
message(STATUS "SDK Mac table overlay written to ${SDK_OVERLAY}")

# ---------------------------------------------------------------------------------------------------------------------------------
set(SRC "${MS_ROOT}/src/red4ext")
set(MODSETTINGS_SOURCES
    ${SRC}/Main.cpp
    ${SRC}/Utils.cpp
    ${SRC}/ModSettings.cpp
    ${SRC}/Variable.cpp
    ${SRC}/IRuntimeVariable.cpp
    ${SRC}/RuntimeVariable.cpp
    ${SRC}/ModConfigVar.cpp
    ${SRC}/ActionNameEx.cpp
    ${SRC}/Hooks/ApplyOverrides.cpp
    ${SRC}/Hooks/ReleaseScriptData.cpp
    ${SRC}/Scripting/RTTIRegistrar.cpp
    ${SRC}/stdafx.cpp
    MacShims.cpp
    MacEntry.cpp
    MacBundle.cpp
    MacVault.cpp
    MacTrap.cpp
)

add_library(mod_settings SHARED ${MODSETTINGS_SOURCES})
set_target_properties(mod_settings PROPERTIES OUTPUT_NAME "mod_settings" PREFIX "" SUFFIX ".dylib")

target_compile_definitions(mod_settings PRIVATE
    RED4EXT_PLATFORM_MACOS
    MOD_SETTINGS_DLLDIR_EX
    "MOD_VERSION_STR=\"${PROJECT_VERSION}\""
    MOD_VERSION_MAJOR=${MS_VER_MAJOR}
    MOD_VERSION_MINOR=${MS_VER_MINOR}
    MOD_VERSION_PATCH=${MS_VER_PATCH}
)
if(MODSETTINGS_MAC_APPLYOVERRIDES)
    target_compile_definitions(mod_settings PRIVATE MODSETTINGS_MAC_APPLYOVERRIDES=1)
endif()

# Include order matters: the generated Relocation-inl.hpp first, then this folder's overlay (Mac copies of the cpcmake and ArchiveXL
# support headers, and the two fork headers the Mac SDK does not have), then the Mac SDK, then the plugin and RedLib (macos/redlib, a
# patched copy of deps/red_lib/include; nameof comes from deps/red_lib/vendor).
# deps/cpcmake/include, deps/archive_xl/support and deps/red4ext.sdk (jackhumbert's fork SDK) are deliberately NOT on the path.
target_include_directories(mod_settings PRIVATE
    "${SDK_OVERLAY}"
    "${CMAKE_CURRENT_SOURCE_DIR}/overlay"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${SDK_INCLUDE}"
    "${SRC}"
    "${MS_ROOT}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/redlib"
    "${MS_ROOT}/deps/red_lib/vendor"
)

# MacCompat.hpp is force-included in every translation unit: __fastcall and friends, the std headers the Windows precompiled header
# (stdafx.hpp) used to drag in through Windows.h.
target_compile_options(mod_settings PRIVATE
    -include "${CMAKE_CURRENT_SOURCE_DIR}/MacCompat.hpp"
    -fexceptions
    -fvisibility=hidden
    -fvisibility-inlines-hidden
    -Wno-deprecated-declarations
    -Wno-deprecated-volatile
    -Wno-unused-value
    -Wno-microsoft
    -Wno-invalid-offsetof
)

# Fail the build unless the three RED4ext entry points are exported and the signature is ad hoc (the game's loader needs a valid one).
add_custom_command(TARGET mod_settings POST_BUILD
    COMMAND /usr/bin/codesign -f -s - "$<TARGET_FILE:mod_settings>"
    COMMAND /bin/bash "${CMAKE_CURRENT_SOURCE_DIR}/check-undefined.sh" "$<TARGET_FILE:mod_settings>"
    VERBATIM
)

message(STATUS "Mod Settings ${PROJECT_VERSION} configured for macOS (ApplyOverrides hook: ${MODSETTINGS_MAC_APPLYOVERRIDES})")
