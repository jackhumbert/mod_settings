#include "RED4ext/RTTISystem.hpp"
#include "Red/TypeInfo/Registrar.hpp"
#include "Utils.hpp"
#include <RED4ext/Common.hpp>
#include <Windows.h>
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

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    ModSettings::ModSettings::GetInstance();
    break;
  }
  return true;
}

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason,
                                        const RED4ext::Sdk *aSdk) {
  switch (aReason) {
  case RED4ext::EMainReason::Load: {
    ModSettings::sdk = aSdk;
    ModSettings::pluginHandle = aHandle;

    aSdk->logger->Info(aHandle, "Starting up Mod Settings " MOD_VERSION_STR);
    auto ptr = GetModuleHandle(nullptr);
    aSdk->logger->InfoF(aHandle, "Base address: %p", ptr);

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

    Red::TypeInfoRegistrar::RegisterDiscovered();

    for (auto script : {L"packed.reds", L"module.reds"}) {
      if (!aSdk->scripts->Add(aHandle, script))
        aSdk->logger->ErrorF(aHandle, "Could not register %ls with redscript; the menu will be missing", script);
    }
    if (!ArchiveXL::RegisterArchive(aHandle, "ModSettings.archive"))
      aSdk->logger->Error(aHandle, "Could not register ModSettings.archive (is ArchiveXL installed and the file present "
                                   "in the plugin folder?); the menu will be missing or blank");
    aSdk->logger->Info(aHandle, "Attaching hooks");
    ModModuleFactory::GetInstance().Load(aSdk, aHandle);
    aSdk->logger->Info(aHandle, "Mod Settings loaded");
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
