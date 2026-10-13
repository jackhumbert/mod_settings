// macOS only. See MacBundle.hpp. Offsets are the ones R41c-2 checked against the Mac binary (macos/LAYOUT.md); the checks themselves are
// a trimmed copy of Codeware's CheckLayout (cp2077-codeware/macos/MacScriptValidator.cpp), which runs on every boot of the modded game.
#include "MacBundle.hpp"

#include "MacShims.hpp"

#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <mach-o/loader.h>

namespace
{
using ModSettingsMac::SafeRead;

// Inside one of the segments of the game executable (MH_EXECUTE). The image slides; the segments' vmaddr are relative to 0x100000000.
bool InGameImage(uint64_t aPtr)
{
    const auto base = ModSettingsMac::GameBase();
    const auto* header = reinterpret_cast<const mach_header_64*>(base);
    const auto* cmd = reinterpret_cast<const load_command*>(header + 1);
    for (uint32_t i = 0; i < header->ncmds; ++i)
    {
        if (cmd->cmd == LC_SEGMENT_64)
        {
            const auto* seg = reinterpret_cast<const segment_command_64*>(cmd);
            if (seg->vmsize && std::strncmp(seg->segname, "__PAGEZERO", 16) != 0)
            {
                const uint64_t start = base + (seg->vmaddr - 0x100000000ull);
                if (aPtr >= start && aPtr < start + seg->vmsize)
                {
                    return true;
                }
            }
        }
        cmd = reinterpret_cast<const load_command*>(reinterpret_cast<const uint8_t*>(cmd) + cmd->cmdsize);
    }
    return false;
}

bool PrintableName(RED4ext::CName aName)
{
    if (!aName.hash)
    {
        return false;
    }
    const char* s = aName.ToString();
    if (!s || !*s)
    {
        return false;
    }
    for (size_t i = 0; i < 256; ++i)
    {
        const auto c = static_cast<unsigned char>(s[i]);
        if (c == 0)
        {
            return i > 0;
        }
        if (c < 0x20 || c > 0x7E)
        {
            return false;
        }
    }
    return false;
}

std::string Fail(const char* aFormat, ...) __attribute__((format(printf, 1, 2)));
std::string Fail(const char* aFormat, ...)
{
    char buf[160];
    va_list args;
    va_start(args, aFormat);
    std::vsnprintf(buf, sizeof(buf), aFormat, args);
    va_end(args);
    return buf;
}
} // namespace

bool ModSettingsMac::IsGameScriptBundle(const void* aBundle, std::string& aWhy, unsigned* aClassCount)
{
    if (!aBundle)
    {
        aWhy = "bundle is null";
        return false;
    }
    const auto* bytes = static_cast<const uint8_t*>(aBundle);

    // classes: DynArray<ScriptClass*> at +0xD0 (entries +0, capacity +8, size +0xC).
    uint64_t entries = 0;
    uint32_t capacity = 0;
    uint32_t size = 0;
    if (!SafeRead(bytes + 0xD0, entries) || !SafeRead(bytes + 0xD8, capacity) || !SafeRead(bytes + 0xDC, size))
    {
        aWhy = "classes (+0xD0) unreadable";
        return false;
    }
    if (size < 1 || size > 200000 || capacity < size || entries == 0 || (entries & 7) != 0)
    {
        aWhy = Fail("classes size=%u capacity=%u entries=0x%llx", size, capacity, static_cast<unsigned long long>(entries));
        return false;
    }

    const uint32_t sample = size < 5 ? size : 5;
    for (uint32_t i = 0; i < sample; ++i)
    {
        uint64_t ptr = 0;
        if (!SafeRead(reinterpret_cast<const void*>(entries + 8ull * i), ptr) || ptr == 0 || (ptr & 7) != 0)
        {
            aWhy = Fail("classes[%u] pointer", i);
            return false;
        }
        uint64_t vtable = 0;
        uint64_t nameHash = 0;
        uint64_t parent = 0;
        uint8_t flags = 0;
        uint32_t propCapacity = 0;
        uint32_t propSize = 0;
        uint64_t getType = 0;
        if (!SafeRead(reinterpret_cast<const void*>(ptr), vtable) || !InGameImage(vtable))
        {
            aWhy = Fail("classes[%u] vtable is not in the game image", i);
            return false;
        }
        if (!SafeRead(reinterpret_cast<const void*>(ptr + 0x08), nameHash) || !PrintableName(RED4ext::CName(nameHash)))
        {
            aWhy = Fail("classes[%u] name (+0x08) is not a printable string", i);
            return false;
        }
        if (!SafeRead(reinterpret_cast<const void*>(ptr + 0x20), parent) || (parent != 0 && (parent & 7) != 0))
        {
            aWhy = Fail("classes[%u] parent (+0x20)", i);
            return false;
        }
        if (!SafeRead(reinterpret_cast<const void*>(ptr + 0x89), flags))
        {
            aWhy = Fail("classes[%u] flags (+0x89)", i);
            return false;
        }
        if (!SafeRead(reinterpret_cast<const void*>(ptr + 0x30), propCapacity) || !SafeRead(reinterpret_cast<const void*>(ptr + 0x34), propSize) ||
            propSize > propCapacity || propSize > 1000000)
        {
            aWhy = Fail("classes[%u] properties (+0x28)", i);
            return false;
        }
        // GetType() is the first virtual after the two destructors: vtable +0x18, a game function.
        if (!SafeRead(reinterpret_cast<const void*>(vtable + 0x18), getType) || !InGameImage(getType))
        {
            aWhy = Fail("classes[%u] vtable GetType slot", i);
            return false;
        }
    }

    // definitionsByName: HashMap<CName, ScriptDefinition*> at +0 (indexTable +0, size +8, capacity +0xC). It must find the two roots.
    uint64_t indexTable = 0;
    uint32_t mapSize = 0;
    uint32_t mapCapacity = 0;
    if (!SafeRead(bytes, indexTable) || !SafeRead(bytes + 8, mapSize) || !SafeRead(bytes + 12, mapCapacity) || indexTable == 0 || mapSize == 0 ||
        mapCapacity < mapSize)
    {
        aWhy = "definitionsByName header (+0x00)";
        return false;
    }
    const auto* definitions = static_cast<const RED4ext::HashMap<RED4ext::CName, void*>*>(aBundle);
    if (!definitions->Get(RED4ext::CName("ISerializable")))
    {
        aWhy = "definitionsByName does not find ISerializable";
        return false;
    }
    if (!definitions->Get(RED4ext::CName("IScriptable")))
    {
        aWhy = "definitionsByName does not find IScriptable";
        return false;
    }

    if (aClassCount)
    {
        *aClassCount = size;
    }
    return true;
}
