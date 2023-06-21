#include "RED4ext/RTTISystem.hpp"
#include "Red/TypeInfo/Registrar.hpp"
#include "Utils.hpp"
#include <RED4ext/Common.hpp>
#include <Windows.h>
#include <stdio.h>


#include "ModSettings.hpp"

#include <RedLib.hpp>
#include <ArchiveXL.hpp>
#include <Registrar.hpp>

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

    auto scriptsFolder = Utils::GetRootDir() / "r6" / "scripts" / "mod_settings";
    if (std::filesystem::exists(scriptsFolder)) {
      aSdk->logger->Info(aHandle, "Deleting old scripts folder");
      std::filesystem::remove_all(scriptsFolder);
    }
    auto archive = Utils::GetRootDir() / "archive" / "pc" / "mod" / "ModSettings.archive";
    if (std::filesystem::exists(archive)) {
      aSdk->logger->Info(aHandle, "Deleting old archive");
      std::filesystem::remove_all(archive);
    }
    auto archiveXL = Utils::GetRootDir() / "archive" / "pc" / "mod" / "ModSettings.archive.xl";
    if (std::filesystem::exists(archiveXL)) {
      aSdk->logger->Info(aHandle, "Deleting old archive.xl");
      std::filesystem::remove_all(archiveXL);
    }
    auto XL = Utils::GetRootDir() / "archive" / "pc" / "mod" / "ModSettings.xl";
    if (std::filesystem::exists(XL)) {
      aSdk->logger->Info(aHandle, "Deleting old xl");
      std::filesystem::remove_all(XL);
    }

    Red::TypeInfoRegistrar::RegisterDiscovered();

    aSdk->scripts->Add(aHandle, L"packed.reds");
    aSdk->scripts->Add(aHandle, L"module.reds");
    ArchiveXL::RegisterArchive(aHandle, "ModSettings.archive");
    ModModuleFactory::GetInstance().Load(aSdk, aHandle);
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
  aInfo->runtime = RED4EXT_RUNTIME_LATEST;
  aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports() { return RED4EXT_API_VERSION_LATEST; }
