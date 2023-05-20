#pragma once
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

struct ScriptProperty;

class IModRuntimeSettingsVar : public RED4ext::user::RuntimeSettingsVar {
public:
  IModRuntimeSettingsVar();
  IModRuntimeSettingsVar(ScriptProperty* prop);

  // custom virtuals
  virtual void __fastcall GetValueToWrite(char *value) = 0;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() = 0;
  virtual void __fastcall UpdateDefault(void *value) = 0;
};
