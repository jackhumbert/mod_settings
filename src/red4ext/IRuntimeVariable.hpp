#pragma once
#include "RED4ext/CName.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

namespace ModSettings {

struct ScriptProperty;

class IRuntimeVariable : public RED4ext::user::RuntimeSettingsVar {
public:
  IRuntimeVariable();
  IRuntimeVariable(ScriptProperty* prop);
  IRuntimeVariable(RED4ext::CName modName, RED4ext::CName className, RED4ext::CName propertyName, RED4ext::CName displayName, RED4ext::CName description, uint32_t order);

  // custom virtuals
  virtual void __fastcall GetValueToWrite(char *value) = 0;
  virtual RED4ext::ScriptInstance *__fastcall GetRequestedValue() = 0;
  virtual RED4ext::ScriptInstance *__fastcall GetAcceptedValue() = 0;
  virtual void __fastcall SetDefaultValue(void *value) = 0;

  RED4ext::CName modName;
  RED4ext::CName className;
};

}