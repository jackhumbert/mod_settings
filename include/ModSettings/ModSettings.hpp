#pragma once

#include <RED4ext/Common.hpp>

#ifdef MOD_SETTINGS_DLLDIR_EX
#define MOD_SETTINGS_DLLDIR __declspec(dllexport) // export DLL information
#else
#define MOD_SETTINGS_DLLDIR __declspec(dllimport) // import DLL information
#endif

namespace ModSettings {

using namespace RED4ext;

struct ModVariable;

struct MOD_SETTINGS_DLLDIR ModSettingDependency {
  static ModSettingDependency* FromString(std::string str, CName scriptClass);

  CName className;
  CName propertyName;
  CString value;
  ModVariable * variable;
};

union MOD_SETTINGS_DLLDIR ModVariableType {
  bool b;
  uint32_t u32;
  int32_t i32;
  float f32;
};

typedef void (__stdcall *runtime_class_callback_t)(CName categoryName, CName propertyName, ModVariableType value);

struct MOD_SETTINGS_DLLDIR Variable  {
  const char * modName;
  const char * className;
  const char * categoryName;
  const char * propertyName;
  CName type;
  const char * displayName;
  const char * description;
  uint32_t order;
  ModVariableType defaultValue;
  ModVariableType stepValue;
  ModVariableType minValue;
  ModVariableType maxValue;
  ModSettingDependency dependency;
  runtime_class_callback_t callback;
};

extern "C" MOD_SETTINGS_DLLDIR void AddVariable(Variable *variable);

} // namespace ModSettings