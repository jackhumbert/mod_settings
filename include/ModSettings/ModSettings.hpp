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

struct ModSettingDependency {
  // void Resolve(std::string str, CName scriptClass);

  CName className;
  CName propertyName;
  CString value;
  ModVariable * variable;
};

union ModVariableType {
  bool b;
  uint32_t u32;
  int32_t i32;
  float f32;
};

typedef std::function<void (CName categoryName, CName propertyName, ModVariableType value)> runtime_class_callback_t;

struct Variable  {
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
  std::shared_ptr<runtime_class_callback_t> callback;
  ModSettingDependency dependency;
};

RED4EXT_ASSERT_SIZE(Variable, 152);

extern "C" MOD_SETTINGS_DLLDIR void AddVariable(Variable *variable);

} // namespace ModSettings