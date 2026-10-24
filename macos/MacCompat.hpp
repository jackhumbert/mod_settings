#pragma once
// macOS only. Force-included into every translation unit of the Mac build (macos/CMakeLists.txt: -include).
//
// The Windows build gets these from <Windows.h> through the precompiled header src/red4ext/stdafx.hpp. On the Mac the SDK's
// RED4ext/Detail/WinCompat.hpp supplies the Windows types, atomics and __declspec; this adds the rest of what the plugin sources
// spell the MSVC way, so src/red4ext keeps its Windows spelling and needs no per-line edits.

#ifdef __APPLE__

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <RED4ext/Detail/WinCompat.hpp>

// MSVC keywords. The calling convention is meaningless on arm64 (one AAPCS64 convention).
#ifndef __fastcall
#define __fastcall
#endif
#ifndef __unaligned
#define __unaligned
#endif
#ifndef __int8
#define __int8 char
#endif
#ifndef __int16
#define __int16 short
#endif
#ifndef __int32
#define __int32 int
#endif
#ifndef __int64
#define __int64 long long
#endif

// Bit-flag operators for an enum class (winnt.h macro, used by the fork's userRuntimeSettingsVar.hpp).
#ifndef DEFINE_ENUM_FLAG_OPERATORS
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE)                                                                                   \
    inline constexpr ENUMTYPE operator|(ENUMTYPE a, ENUMTYPE b) noexcept                                                       \
    {                                                                                                                          \
        return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) | ((std::underlying_type_t<ENUMTYPE>)b));                       \
    }                                                                                                                          \
    inline ENUMTYPE& operator|=(ENUMTYPE& a, ENUMTYPE b) noexcept                                                              \
    {                                                                                                                          \
        return (ENUMTYPE&)(((std::underlying_type_t<ENUMTYPE>&)a) |= ((std::underlying_type_t<ENUMTYPE>)b));                   \
    }                                                                                                                          \
    inline constexpr ENUMTYPE operator&(ENUMTYPE a, ENUMTYPE b) noexcept                                                       \
    {                                                                                                                          \
        return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) & ((std::underlying_type_t<ENUMTYPE>)b));                       \
    }                                                                                                                          \
    inline ENUMTYPE& operator&=(ENUMTYPE& a, ENUMTYPE b) noexcept                                                              \
    {                                                                                                                          \
        return (ENUMTYPE&)(((std::underlying_type_t<ENUMTYPE>&)a) &= ((std::underlying_type_t<ENUMTYPE>)b));                   \
    }                                                                                                                          \
    inline constexpr ENUMTYPE operator~(ENUMTYPE a) noexcept                                                                   \
    {                                                                                                                          \
        return ENUMTYPE(~((std::underlying_type_t<ENUMTYPE>)a));                                                               \
    }
#endif

// The SDK headers include each other in a circle (RTTISystem.hpp -> RTTITypes.hpp -> NativeTypes-inl.hpp uses CRTTISystem). They
// only compile when the umbrella header comes first, so every translation unit gets it first (the Windows stdafx.hpp precompiled
// header did the same).
#include <RED4ext/RED4ext.hpp>

#endif // __APPLE__
