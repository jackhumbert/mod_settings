#pragma once
// macOS copy of cp2077-archive-xl-macos/support/red4ext/ArchiveXL.hpp (which is the Windows header unchanged: <libloaderapi.h>,
// GetModuleFileNameW). GetModulePath and Initialize()/RegisterPathOrQueue differ. Lives in macos/overlay so the repo's deps/archive_xl is not edited.

#include <filesystem>
#include <vector>

#include <cstdio>
#include <dlfcn.h>

#include <RED4ext/Detail/WinCompat.hpp>

#include <RED4ext/Callback.hpp>
#include <RED4ext/Memory/Allocators.hpp>
#include <RED4ext/RTTISystem.hpp>

class ArchiveXL
{
public:
    inline static bool RegisterArchive(std::filesystem::path aPath)
    {
        std::error_code error;

        if (!std::filesystem::exists(aPath, error) || !std::filesystem::is_regular_file(aPath, error))
        {
            return false;
        }

        return RegisterPathOrQueue(std::move(aPath));
    }

    inline static bool RegisterArchive(HMODULE aHandle, std::filesystem::path aPath)
    {
        if (aPath.is_relative())
        {
            aPath = GetModulePath(aHandle) / aPath;
        }

        return RegisterArchive(std::move(aPath));
    }

    inline static bool RegisterArchives(std::filesystem::path aPath)
    {
        std::error_code error;

        if (!std::filesystem::exists(aPath, error) || !std::filesystem::is_directory(aPath, error))
        {
            return false;
        }

        return RegisterPathOrQueue(std::move(aPath));
    }

    inline static bool RegisterArchives(HMODULE aHandle, std::filesystem::path aPath)
    {
        if (aPath.is_relative())
        {
            aPath = GetModulePath(aHandle) / aPath;
        }

        return RegisterArchives(std::move(aPath));
    }

private:
    ArchiveXL() = default;

    inline static bool Initialize()
    {
        if (s_facade)
            return true;

#if !defined(__APPLE__)
        if (!RED4ext::Memory::Vault::Get()->poolRegistry.Get(RED4ext::Memory::PoolEngine::Name))
            return false;
#else
        // macOS: the plugin's Memory::Vault is a stand-in with the four RTTI/script pools only (MacVault.cpp), so the engine pool is never in
        // its registry and this check could never pass. The plugin calls RegisterArchive from modsettings_registerNatives, when the game's RTTI
        // system exists and ArchiveXL's natives are registered; the RTTI system itself is the readiness test here.
        if (!RED4ext::CRTTISystem::Get())
            return false;
#endif

        auto facade = RED4ext::CRTTISystem::Get()->GetClass("ArchiveXL");

        if (!facade || !facade->GetFunction("Version"))
            return false;

        s_facade = facade;
        return true;
    }

    inline static bool RegisterPathOrQueue(std::filesystem::path aPath)
    {
        if (!Initialize())
        {
#if defined(__APPLE__)
            // The queue is flushed by an RTTI post-register callback, and the game never calls those callbacks on macOS: say so.
            std::fprintf(stderr, "[mod_settings] ArchiveXL is not ready (class ArchiveXL with Version not registered): archive %s is queued and will not be registered\n",
                         aPath.string().c_str());
#endif
            s_paths.push_back(std::move(aPath));

            if (s_paths.size() == 1)
            {
                RegisterPendingPaths();
            }

            return true;
        }

        return RegisterPath(aPath);
    }

    inline static bool RegisterPath(const std::filesystem::path& aPath)
    {
        bool success;
        RED4ext::CString path(aPath.string());

        auto rtti = RED4ext::CRTTISystem::Get();
        RED4ext::CStackType arg(rtti->GetType("String"), &path);
        RED4ext::CStackType result(rtti->GetType("Bool"), &success);
        RED4ext::CStack stack(nullptr, &arg, 1, &result);

        std::error_code error;
        RED4ext::CName method(std::filesystem::is_directory(aPath, error) ? "RegisterDir" : "RegisterArchive");

        s_facade->GetFunction(method)->Execute(&stack);

        return success;
    }

    static inline void RegisterPendingPaths()
    {
        if (Initialize())
        {
            for (const auto& path : s_paths)
            {
                RegisterPath(path);
            }

            s_paths.clear();
        }
        else
        {
            RED4ext::CRTTISystem::Get()->AddPostRegisterCallback(&RegisterPendingPaths);
        }
    }

    inline static std::filesystem::path GetModulePath(HMODULE aHandle)
    {
        // macOS. aHandle is the plugin handle RED4ext gave the plugin; the Windows GetModuleFileNameW(handle) has no direct
        // counterpart. This function is an inline member, compiled into the plugin that calls it (-fvisibility-inlines-hidden keeps
        // one copy per dylib), so dladdr on its own address names the calling plugin's dylib and the folder is the plugin folder.
        (void)aHandle;
        Dl_info info{};
        if (dladdr(reinterpret_cast<const void*>(&ArchiveXL::GetModulePath), &info) && info.dli_fname)
        {
            std::error_code error;
            auto path = std::filesystem::absolute(std::filesystem::path(info.dli_fname), error);
            return (error ? std::filesystem::path(info.dli_fname) : path).parent_path();
        }
        return {};
    }

    inline static RED4ext::CClass* s_facade;
    inline static std::vector<std::filesystem::path> s_paths;
};
