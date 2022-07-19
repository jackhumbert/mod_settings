#include <RED4ext/InstanceType.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/RTTITypes.hpp>
#include <RED4ext/RED4ext.hpp>
#include <spdlog/spdlog.h>
#include <RED4ext/Scripting/Natives/Generated/EInputKey.hpp>
#include "Utils.hpp"
#include <stdio.h>
#include <Windows.h>
#include <Dbt.h>

#include "ModRuntimeSettingsVar.hpp"
#include "ModSettings.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "ScriptDefinitions/ScriptHost.hpp"
#include "stdafx.hpp"
#include <RED4ext/RED4ext.hpp>

struct ScriptRTTIContainer {
  RED4ext::CRTTISystem *rtti;
  void *scriptContainerVft;
  ScriptHost *host;
};

struct ScriptData {
  RED4ext::HashMap<uint64_t, uint64_t> unk00;
  RED4ext::HashMap<uint64_t, uint64_t> unk30;
  RED4ext::HashMap<uint64_t, uint64_t> unk60;
  RED4ext::DynArray<void *> files;
  RED4ext::DynArray<void *> functions;
  RED4ext::DynArray<void *> enums;
  RED4ext::DynArray<void *> unkC0;
  RED4ext::DynArray<ScriptClass *> classes;
  RED4ext::DynArray<void *> types;
  RED4ext::DynArray<RED4ext::CString> strings;
  RED4ext::HashMap<uint64_t, uint64_t> unk100;
  uint8_t unk60MUTX;
  void *unk138;
};

bool __fastcall ProcessScriptTypes(uint32_t *version, ScriptData *scriptData, void *scriptLogger);
constexpr uintptr_t ProcessScriptTypesAddr = 0x272560 + 0xC00;
decltype(&ProcessScriptTypes) ProcessScriptTypes_Original;

bool __fastcall ProcessScriptTypes(uint32_t *version, ScriptData *scriptData, void *scriptLogger) {
  ModSettings::ReadFromFile();
  for (const auto &scriptClass : scriptData->classes) {
    for (const auto &prop : scriptClass->properties) {
      if (prop->runtimeProperties.size) {
        auto mod = prop->runtimeProperties.Get("ModSettings.mod");
        if (mod) {
          auto variable =
              (ModSettingsVariable *)RED4ext::CRTTISystem::Get()->GetClass("ModSettings")->AllocInstance(true);
          variable->mod = RED4ext::CNamePool::Add(mod->c_str());
          variable->typeName = prop->type->name;
          variable->className = scriptClass->name;

          auto category = prop->runtimeProperties.Get("ModSettings.category");
          if (category) {
            variable->category = RED4ext::CNamePool::Add(category->c_str());
          } else {
            variable->category = "None";
          }

          ModRuntimeSettingsVar *settingsVar = NULL;
          auto propType = RED4ext::CRTTISystem::Get()->GetType(prop->type->name);

          if (prop->type->name == "Bool") {
            settingsVar = new ModRuntimeSettingsVarBool(prop);
          } else if (prop->type->name == "Float") {
            settingsVar = new ModRuntimeSettingsVarFloat(prop);
          } else if (prop->type->name == "Int32" || prop->type->name == "Uint32") {
            settingsVar = new ModRuntimeSettingsVarInt32(prop);
          } else if (propType->GetType() == RED4ext::ERTTIType::Enum) {
            settingsVar = new ModRuntimeSettingsVarEnum(prop);
          }

          if (settingsVar) {
            variable->settingsVar = settingsVar;
            ModSettings::AddVariable(variable);
          }
        }
      }
    }
  }
  auto og = ProcessScriptTypes_Original(version, scriptData, scriptLogger);
  return og;
}

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason,
                                        const RED4ext::Sdk *aSdk) {
  switch (aReason) {
  case RED4ext::EMainReason::Load: {
    // Attach hooks, register RTTI types, add custom states or initalize your
    // application. DO NOT try to access the game's memory at this point, it
    // is not initalized yet.

    Utils::CreateLogger();
    spdlog::info("Starting up");

    RED4ext::RTTIRegistrator::Add(ModSettings::RegisterTypes, ModSettings::PostRegisterTypes);

    while (!aSdk->hooking->Attach(aHandle, RED4EXT_OFFSET_TO_ADDR(ProcessScriptTypesAddr), &ProcessScriptTypes,
                                  reinterpret_cast<void **>(&ProcessScriptTypes_Original)));

    break;
  }
  case RED4ext::EMainReason::Unload: {
    // Free memory, detach hooks.
    // The game's memory is already freed, to not try to do anything with it.

    spdlog::info("Shutting down");
    aSdk->hooking->Detach(aHandle, RED4EXT_OFFSET_TO_ADDR(ProcessScriptTypesAddr));
    spdlog::shutdown();
    break;
  }
  }

  return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo *aInfo) {
  aInfo->name = L"Mod Settings";
  aInfo->author = L"Jack Humbert";
  aInfo->version = RED4EXT_SEMVER(0, 0, 1);
  aInfo->runtime = RED4EXT_RUNTIME_LATEST;
  aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports() { return RED4EXT_API_VERSION_LATEST; }
