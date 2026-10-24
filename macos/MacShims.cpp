// macOS only. See MacShims.hpp.
#include "MacShims.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_set>

#include <dlfcn.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach/mach.h>
#include <mach/vm_map.h>

namespace
{
const RED4ext::Sdk* g_sdk = nullptr;
RED4ext::PluginHandle g_handle = nullptr;

// Any address inside this dylib will do for dladdr.
void ModuleMarker() {}

enum class Level
{
    Info,
    Warn,
    Error
};

void Emit(Level aLevel, const char* aFormat, va_list aArgs)
{
    char line[2048];
    std::vsnprintf(line, sizeof(line), aFormat, aArgs);
    std::fprintf(stderr, "[mod_settings] %s\n", line);
    if (g_sdk && g_sdk->logger)
    {
        // The line may contain a '%' (a path, a name), so it goes in as a message, never as the format.
        switch (aLevel)
        {
        case Level::Info:
            if (g_sdk->logger->Info)
                g_sdk->logger->Info(g_handle, line);
            break;
        case Level::Warn:
            if (g_sdk->logger->Warn)
                g_sdk->logger->Warn(g_handle, line);
            break;
        case Level::Error:
            if (g_sdk->logger->Error)
                g_sdk->logger->Error(g_handle, line);
            break;
        }
    }
}

using PeerAttach = bool (*)(uintptr_t aTarget, void* aDetour, void** aOriginal);
} // namespace

void ModSettingsMac::Init(const RED4ext::Sdk* aSdk, RED4ext::PluginHandle aHandle)
{
    g_sdk = aSdk;
    g_handle = aHandle;
}

const RED4ext::Sdk* ModSettingsMac::Sdk()
{
    return g_sdk;
}

RED4ext::PluginHandle ModSettingsMac::Handle()
{
    return g_handle;
}

void ModSettingsMac::Log(const char* aFormat, ...)
{
    va_list args;
    va_start(args, aFormat);
    Emit(Level::Info, aFormat, args);
    va_end(args);
}

void ModSettingsMac::LogWarn(const char* aFormat, ...)
{
    va_list args;
    va_start(args, aFormat);
    Emit(Level::Warn, aFormat, args);
    va_end(args);
}

void ModSettingsMac::LogError(const char* aFormat, ...)
{
    va_list args;
    va_start(args, aFormat);
    Emit(Level::Error, aFormat, args);
    va_end(args);
}

void ModSettingsMac::LogOnce(const char* aKey, const char* aFormat, ...)
{
    static std::mutex lock;
    static std::unordered_set<std::string> seen;
    {
        std::scoped_lock guard(lock);
        if (!seen.insert(aKey).second)
        {
            return;
        }
    }
    va_list args;
    va_start(args, aFormat);
    Emit(Level::Warn, aFormat, args);
    va_end(args);
}

bool ModSettingsMac::DiagEnabled()
{
    static const bool enabled = [] {
        const char* value = std::getenv("MODSETTINGS_MAC_DIAG");
        return value && value[0] == '1';
    }();
    return enabled;
}

std::filesystem::path ModSettingsMac::ModulePath()
{
    Dl_info info{};
    if (dladdr(reinterpret_cast<const void*>(&ModuleMarker), &info) && info.dli_fname)
    {
        std::error_code error;
        auto path = std::filesystem::absolute(std::filesystem::path(info.dli_fname), error);
        return error ? std::filesystem::path(info.dli_fname) : path;
    }
    return {};
}

std::filesystem::path ModSettingsMac::PluginDir()
{
    return ModulePath().parent_path();
}

std::filesystem::path ModSettingsMac::GameRoot()
{
    // .../<root>/red4ext/plugins/mod_settings/mod_settings.dylib
    return PluginDir().parent_path().parent_path().parent_path();
}

uintptr_t ModSettingsMac::GameBase()
{
    static const uintptr_t base = [] {
        const uint32_t count = _dyld_image_count();
        for (uint32_t i = 0; i < count; ++i)
        {
            const auto* header = reinterpret_cast<const mach_header_64*>(_dyld_get_image_header(i));
            if (header && header->filetype == MH_EXECUTE)
            {
                return reinterpret_cast<uintptr_t>(header);
            }
        }
        return reinterpret_cast<uintptr_t>(_dyld_get_image_header(0));
    }();
    return base;
}

void* ModSettingsMac::LoadedModule(const char* aPath)
{
    return aPath ? dlopen(aPath, RTLD_LAZY | RTLD_NOLOAD) : nullptr;
}

void* ModSettingsMac::Symbol(void* aModule, const char* aName)
{
    return aModule ? dlsym(aModule, aName) : nullptr;
}

void* ModSettingsMac::AnySymbol(const char* aName)
{
    return dlsym(RTLD_DEFAULT, aName);
}

bool ModSettingsMac::SafeCopy(const void* aSrc, void* aDst, std::size_t aSize)
{
    vm_size_t got = 0;
    return vm_read_overwrite(mach_task_self(), reinterpret_cast<vm_address_t>(aSrc), aSize, reinterpret_cast<vm_address_t>(aDst), &got) ==
               KERN_SUCCESS &&
           got == aSize;
}

std::string ModSettingsMac::WideToUtf8(const wchar_t* aText)
{
    std::string out;
    if (!aText)
    {
        return out;
    }
    for (const wchar_t* p = aText; *p; ++p)
    {
        uint32_t c = static_cast<uint32_t>(*p);
        if (c < 0x80)
        {
            out += static_cast<char>(c);
        }
        else if (c < 0x800)
        {
            out += static_cast<char>(0xC0 | (c >> 6));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
        else if (c < 0x10000)
        {
            out += static_cast<char>(0xE0 | (c >> 12));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
        else
        {
            out += static_cast<char>(0xF0 | (c >> 18));
            out += static_cast<char>(0x80 | ((c >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((c >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (c & 0x3F));
        }
    }
    return out;
}

std::wstring ModSettingsMac::Utf8ToWide(const char* aText)
{
    std::wstring out;
    if (!aText)
    {
        return out;
    }
    const auto* p = reinterpret_cast<const unsigned char*>(aText);
    while (*p)
    {
        uint32_t c = *p++;
        int extra = 0;
        if (c >= 0xF0)
        {
            c &= 0x07;
            extra = 3;
        }
        else if (c >= 0xE0)
        {
            c &= 0x0F;
            extra = 2;
        }
        else if (c >= 0xC0)
        {
            c &= 0x1F;
            extra = 1;
        }
        else if (c >= 0x80)
        {
            out += L'?'; // a stray continuation byte
            continue;
        }
        for (int i = 0; i < extra && (*p & 0xC0) == 0x80; ++i)
        {
            c = (c << 6) | (*p++ & 0x3F);
        }
        out += static_cast<wchar_t>(c);
    }
    return out;
}

const char* ModSettingsMac::HookResultName(HookResult aResult)
{
    switch (aResult)
    {
    case HookResult::Attached:
        return "ATTACHED";
    case HookResult::Chained:
        return "CHAINED";
    case HookResult::NotActive:
        return "NOT ACTIVE (offset is not on the RED4ext allowlist)";
    case HookResult::Refused:
        return "REFUSED";
    case HookResult::Failed:
        return "FAILED";
    }
    return "?";
}

ModSettingsMac::HookResult ModSettingsMac::AttachHook(const char* aName, uintptr_t aAddress, void* aDetour, void** aOriginal,
                                                      bool aSharedWithCodeware)
{
    const auto rva = static_cast<unsigned long>(aAddress - GameBase());

    // R42f (diagnostics): MODSETTINGS_MAC_NOHOOKS=all, or a comma list of hook names (substring match), leaves those hooks out.
    // Unset = every hook as before.
    if (const char* skip = std::getenv("MODSETTINGS_MAC_NOHOOKS"))
    {
        if (std::strcmp(skip, "all") == 0 || std::strstr(skip, aName) != nullptr)
        {
            Log("hook %s SKIPPED by MODSETTINGS_MAC_NOHOOKS=%s", aName, skip);
            return HookResult::Refused;
        }
    }

    if (!aAddress || IsTrapAddress(aAddress))
    {
        LogWarn("hook %s: the hash has no Mac address, nothing to hook (feature skipped)", aName);
        return HookResult::Refused;
    }
    if (!aDetour)
    {
        LogError("hook %s: null detour", aName);
        return HookResult::Refused;
    }
    if (!g_sdk || !g_sdk->hooking || !g_sdk->hooking->Attach)
    {
        LogError("hook %s: RED4ext has no hooking interface", aName);
        return HookResult::Failed;
    }

    // 1. Codeware also hooks this target: it owns it (it installs its hooks in its own Main, before this runs). Join its chain: the detour
    //    runs first and *aOriginal is the previous detour in the chain. If Codeware is loaded and does not accept the target, its hook is
    //    not there; do not attach behind its back.
    if (aSharedWithCodeware)
    {
        if (auto fn = reinterpret_cast<PeerAttach>(AnySymbol("codeware_mac_hook_chain_attach")))
        {
            if (fn(aAddress, aDetour, aOriginal))
            {
                Log("hook %s rva=0x%lx: CHAINED on codeware_mac_hook_chain_attach (original=%p)", aName, rva, aOriginal ? *aOriginal : nullptr);
                return HookResult::Chained;
            }
            LogError("hook %s rva=0x%lx: REFUSED. Codeware is loaded but does not own this target (its hook did not install), so it cannot be "
                     "chained, and attaching directly would race it. The feature is skipped.",
                     aName, rva);
            return HookResult::Refused;
        }
        // No Codeware symbol: Codeware is not loaded. Fall through to a direct attach.
        Log("hook %s rva=0x%lx: Codeware is not loaded, attaching directly", aName, rva);
    }
    else
    {
        // Another plugin (ArchiveXL, Codeware) may own an ordinary target too; ask them first. A plugin that does not own it answers false.
        static const char* const kPeers[] = {"codeware_mac_hook_chain_attach", "archivexl_mac_hook_chain_attach"};
        for (const char* peer : kPeers)
        {
            if (auto fn = reinterpret_cast<PeerAttach>(AnySymbol(peer)))
            {
                if (fn(aAddress, aDetour, aOriginal))
                {
                    Log("hook %s rva=0x%lx: CHAINED on %s (original=%p)", aName, rva, peer, aOriginal ? *aOriginal : nullptr);
                    return HookResult::Chained;
                }
            }
        }
    }

    // 2. One attempt, straight through RED4ext.
    const bool ok = g_sdk->hooking->Attach(g_handle, reinterpret_cast<void*>(aAddress), aDetour, aOriginal);
    if (!ok)
    {
        LogError("hook %s rva=0x%lx: FAILED (RED4ext refused the attach)", aName, rva);
        return HookResult::Failed;
    }
    if (aOriginal && *aOriginal == reinterpret_cast<void*>(aAddress))
    {
        // RED4ext-macos reports success for an offset that is not on RED4EXT_GUM_HOOK_OFFSETS and hands back the target itself.
        LogError("hook %s rva=0x%lx: %s. Add the offset to GUM_OFFSETS.txt (macos/hook-offsets.txt).", aName, rva,
                 HookResultName(HookResult::NotActive));
        return HookResult::NotActive;
    }
    Log("hook %s rva=0x%lx: ATTACHED (original=%p)", aName, rva, aOriginal ? *aOriginal : nullptr);
    return HookResult::Attached;
}

void ModSettingsMac::DetachHook(const char* aName, uintptr_t aAddress, HookResult aHow)
{
    if (aHow == HookResult::Chained)
    {
        // The chain owner keeps the stub; this plugin's detour stays in the chain until the process exits.
        Log("hook %s: chained, not detached", aName);
        return;
    }
    if (aHow != HookResult::Attached)
    {
        return;
    }
    if (g_sdk && g_sdk->hooking && g_sdk->hooking->Detach)
    {
        g_sdk->hooking->Detach(g_handle, reinterpret_cast<void*>(aAddress));
    }
}
