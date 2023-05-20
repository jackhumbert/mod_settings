#pragma once
#include "ModSettings.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

struct IModRuntimeSettingsVar : public RED4ext::user::RuntimeSettingsVar {
  inline IModRuntimeSettingsVar() {
    bitfield.isInPreGame = true;
    bitfield.isInGame = true;
    bitfield.isVisible = true;
    bitfield.isInitialized = true;
    bitfield.isDisabled = false;
    bitfield.canBeRestoredToDefault = true;
  }

  inline IModRuntimeSettingsVar(ScriptProperty* prop) : IModRuntimeSettingsVar() {
    updatePolicy = RED4ext::user::EConfigVarUpdatePolicy::ConfirmationRequired;
    name = prop->name;
    auto className = prop->parent->name;
    groupPath = RED4ext::CNamePool::Add("/mods/" + *className.ToString() + *"/" + *name.ToString());

    prop->ReadProperty("ModSettings.displayName", &displayName, name);
    prop->ReadProperty("ModSettings.description", &description);
    prop->ReadProperty("ModSettings.order", &order);
  }

  // custom virtuals
  virtual void __fastcall GetValueToWrite(char *value) = 0;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() = 0;
  virtual void __fastcall UpdateDefault(void *value) = 0;
};
