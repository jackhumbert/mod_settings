#pragma once

#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

struct IModRuntimeSettingsVar;

struct ModSettingsVariable {
  RED4ext::CName mod = RED4ext::CName();
  RED4ext::CName typeName = RED4ext::CName();
  RED4ext::CName className = RED4ext::CName();
  RED4ext::CName category = RED4ext::CName();
  IModRuntimeSettingsVar *settingsVar;
  RED4ext::DynArray<RED4ext::IScriptable *> listeners;
  RED4ext::SharedMutex listeners_lock;

  void UpdateValues();
};