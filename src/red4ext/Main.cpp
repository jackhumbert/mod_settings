#include "RED4ext/RTTISystem.hpp"
#include "Red/TypeInfo/Registrar.hpp"
#include "Utils.hpp"
#include <RED4ext/Common.hpp>
#ifndef __APPLE__
#include <Windows.h>
#else
#include "MacShims.hpp"
#endif
#include <stdio.h>


#include "ModSettings.hpp"

#include <RedLib.hpp>
#include <ArchiveXL.hpp>
#include <CyberpunkMod.hpp>
#include <CNames.hpp>

namespace ModSettings {
const RED4ext::Sdk *sdk;
RED4ext::PluginHandle pluginHandle;
} // namespace ModSettings

#ifndef __APPLE__
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    ModSettings::ModSettings::GetInstance();
    break;
  }
  return true;
}
#endif

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason,
                                        const RED4ext::Sdk *aSdk) {
  switch (aReason) {
  case RED4ext::EMainReason::Load: {
    ModSettings::sdk = aSdk;
    ModSettings::pluginHandle = aHandle;
#ifdef __APPLE__
    ModSettingsMac::Init(aSdk, aHandle);
#endif

    aSdk->logger->Info(aHandle, "Starting up Mod Settings " MOD_VERSION_STR);
#ifdef __APPLE__
    // The Mach-O header of the game executable, not GetModuleHandle(nullptr) (image 0 can be an inserted library).
    aSdk->logger->InfoF(aHandle, "Base address: %p", reinterpret_cast<void *>(ModSettingsMac::GameBase()));
#else
    auto ptr = GetModuleHandle(nullptr);
    aSdk->logger->InfoF(aHandle, "Base address: %p", ptr);
#endif

#ifndef __APPLE__
    // Files from releases that installed outside the plugin folder. Only remove
    // them when this release's own copies are present (a mod manager that moved
    // ours there would otherwise lose the menu), and never throw out of Load on
    // a locked or read-only file: RED4ext does not catch it.
    wchar_t pluginPath[MAX_PATH + 1] = {0};
    GetModuleFileNameW(aHandle, pluginPath, MAX_PATH);
    auto pluginDir = std::filesystem::path(pluginPath).parent_path();
    auto RemoveLegacy = [&](const std::filesystem::path &path, const std::filesystem::path &replacement) {
      std::error_code ec;
      if (!std::filesystem::exists(path, ec))
        return;
      if (!std::filesystem::exists(pluginDir / replacement, ec)) {
        aSdk->logger->WarnF(aHandle, "Keeping %ls: %ls is missing from the plugin folder", path.c_str(),
                            replacement.c_str());
        return;
      }
      aSdk->logger->InfoF(aHandle, "Deleting old %ls", path.c_str());
      std::filesystem::remove_all(path, ec);
      if (ec)
        aSdk->logger->WarnF(aHandle, "Could not delete %ls: %s", path.c_str(), ec.message().c_str());
    };
    auto modArchives = Utils::GetRootDir() / "archive" / "pc" / "mod";
    RemoveLegacy(Utils::GetRootDir() / "r6" / "scripts" / "mod_settings", "packed.reds");
    RemoveLegacy(modArchives / "ModSettings.archive", "ModSettings.archive");
    RemoveLegacy(modArchives / "ModSettings.archive.xl", "ModSettings.archive.xl");
    RemoveLegacy(modArchives / "ModSettings.xl", "ModSettings.archive.xl");

#endif // __APPLE__: the removals above clean up old Windows install locations (r6/scripts, archive/pc/mod); no Mac install ever used them

#ifndef __APPLE__
    Red::TypeInfoRegistrar::RegisterDiscovered();
#endif

#ifdef __APPLE__
    // No aSdk->scripts->Add on the Mac: it looks for packed.reds and module.reds at the plugin root, which does not exist here ("Could not find path
    // relative to plugin"). The Mac scripts are in <plugin folder>/Scripts/ and are compiled offline into the script cache by
    // cp2077-codeware/tools/build-cache.py.
    ModSettingsMac::Log("scripts: not added through RED4ext; they are compiled from <plugin folder>/Scripts/ into the offline script cache");
    // The rest of the Windows Load (register the RedLib types, register the ArchiveXL archive, attach the hooks) runs later on the Mac, from
    // modsettings_registerNatives (macos/MacEntry.cpp): the game never calls the RTTI register callbacks there, and Codeware has to have
    // installed its hooks before ReleaseScriptData can join them.
    ModSettingsMac::Log("loaded; waiting for modsettings_registerNatives (called at script initialisation, after every plugin's Main)");
#else
    for (auto script : {L"packed.reds", L"module.reds"}) {
      if (!aSdk->scripts->Add(aHandle, script))
        aSdk->logger->ErrorF(aHandle, "Could not register %ls with redscript; the menu will be missing", script);
    }
    if (!ArchiveXL::RegisterArchive(aHandle, "ModSettings.archive"))
      aSdk->logger->Error(aHandle, "Could not register ModSettings.archive (is ArchiveXL installed and the file present "
                                   "in the plugin folder?); the menu will be missing or blank");
    aSdk->logger->Info(aHandle, "Attaching hooks");
#endif
#ifndef __APPLE__
    ModModuleFactory::GetInstance().Load(aSdk, aHandle);
    aSdk->logger->Info(aHandle, "Mod Settings loaded");
#endif
    // Engine::RTTIRegistrar::RegisterPending();

    break;
  }
  case RED4ext::EMainReason::Unload: {
    aSdk->logger->Info(aHandle, "Shutting down");
    ModModuleFactory::GetInstance().Unload(aSdk, aHandle);
    break;
  }
  }

  return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo *aInfo) {
  aInfo->name = L"Mod Settings";
  aInfo->author = L"Jack Humbert";
  aInfo->version = RED4EXT_SEMVER(MOD_VERSION_MAJOR, MOD_VERSION_MINOR, MOD_VERSION_PATCH);
  // aInfo->runtime = RED4EXT_RUNTIME_LATEST;
  aInfo->runtime = RED4EXT_RUNTIME_INDEPENDENT;
  aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports() { return RED4EXT_API_VERSION_LATEST; }
