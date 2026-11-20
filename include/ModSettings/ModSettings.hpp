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
  // not yet supported
  CName cname;
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

// updating ModVariableType for CName will change this
// RED4EXT_ASSERT_SIZE(Variable, 152);

// Queues a variable; it is created when the game's scripts are loaded, read from user.ini ([className] propertyName),
// and its callback called with the saved value then and with every change the player accepts. type is "Bool",
// "Int32", "Uint32", "Float", "EInputKey" or the name of an enum type in RTTI (ApiVersion 1 and later): a list of
// the enum's values in order, each shown as the localization key <displayName>-<value name>, its value and
// defaultValue.u32 the index into them. An enum's saved text is a value's name, or an index; a Bool saved under the
// same name (false or true) reads as the first or second value. A type it does not know is logged and the variable is not added.
extern "C" MOD_SETTINGS_DLLDIR void AddVariable(Variable *variable);

// The C++ API's version: 1 adds enum variables. Mod Settings releases without it have version 0 (the export is
// missing), so look it up with GetProcAddress before relying on it.
extern "C" MOD_SETTINGS_DLLDIR uint32_t ApiVersion();

} // namespace ModSettings