#include <RED4ext/InstanceType.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/RTTITypes.hpp>
#include <RED4ext/RED4ext.hpp>
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

#include "Scripting/RTTIClass.hpp"
#include "ModConfigVar.hpp"

const RED4ext::Sdk *sdk;
RED4ext::PluginHandle pluginHandle;

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

//ScriptData *scriptData = NULL;

void __fastcall ProcessModSettings(ScriptData * scriptData) {
  if (scriptData) {
    ModSettings::ReadFromFile();
    ModSettings::ClearVariables();
    for (const auto &scriptClass : scriptData->classes) {
      for (const auto &prop : scriptClass->properties) {
        if (prop->runtimeProperties.size) {
          auto mod = prop->runtimeProperties.Get("ModSettings.mod");
          if (mod) {
            auto variable =
                (ModSettingsVariable *)RED4ext::CRTTISystem::Get()->GetClass("ModSettings")->CreateInstance(true);
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
            } else if (propType && propType->GetType() == RED4ext::ERTTIType::Enum) {
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
  }
} 

// 1.6  RVA: 0x273D50
// 1.61 RVA: 0x274300
// 1.62 RVA: 0x274380
// 48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B D9 E8 99 0B 00 00 48 8D 8B 38 01 00 00 FF 15
void *__fastcall ReleaseScriptData(ScriptData *scriptData);
constexpr uintptr_t ReleaseScriptDataAddr = 0x274380;
decltype(&ReleaseScriptData) ReleaseScriptData_Original;

void *__fastcall ReleaseScriptData(ScriptData *scriptData) {
  ProcessModSettings(scriptData);
  auto og = ReleaseScriptData_Original(scriptData);
  return og;
}

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason,
                                        const RED4ext::Sdk *aSdk) {
  switch (aReason) {
  case RED4ext::EMainReason::Load: {
    // Attach hooks, register RTTI types, add custom states or initalize your
    // application. DO NOT try to access the game's memory at this point, it
    // is not initalized yet.

    sdk = aSdk;
    pluginHandle = aHandle;

    aSdk->logger->Info(aHandle, "Starting up Mod Settings v0.0.12");
    RED4ext::RTTIRegistrator::Add(ModSettings::RegisterTypes, ModSettings::PostRegisterTypes);

    while (!aSdk->hooking->Attach(aHandle, RED4EXT_OFFSET_TO_ADDR(ReleaseScriptDataAddr), &ReleaseScriptData,
                                  reinterpret_cast<void **>(&ReleaseScriptData_Original)))
      ;

    Engine::RTTIRegistrar::RegisterPending();

    break;
  }
  case RED4ext::EMainReason::Unload: {
    // Free memory, detach hooks.
    // The game's memory is already freed, to not try to do anything with it.

    aSdk->logger->Info(aHandle, "Shutting down");
    aSdk->hooking->Detach(aHandle, RED4EXT_OFFSET_TO_ADDR(ReleaseScriptDataAddr));
    break;
  }
  }

  return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo *aInfo) {
  aInfo->name = L"Mod Settings";
  aInfo->author = L"Jack Humbert";
  aInfo->version = RED4EXT_SEMVER(0, 0, 12);
  aInfo->runtime = RED4EXT_RUNTIME_LATEST;
  aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports() { return RED4EXT_API_VERSION_LATEST; }
