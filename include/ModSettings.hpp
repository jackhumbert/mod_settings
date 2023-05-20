#pragma once

#include "ModSettingsVariable.hpp"
#include <ModSettings.hpp>
#include <RED4ext/Common.hpp>


#ifdef MOD_SETTINGS_DLLDIR_EX
#define MOD_SETTINGS_DLLDIR __declspec(dllexport) // export DLL information
#else
#define MOD_SETTINGS_DLLDIR __declspec(dllimport) // import DLL information
#endif

extern const RED4ext::Sdk *sdk;
extern RED4ext::PluginHandle pluginHandle;

struct ScriptData;

struct MOD_SETTINGS_DLLDIR ModSettings : RED4ext::IScriptable {
  RED4ext::CClass *GetNativeType();

  static ModSettings *GetInstance();

  static void ProcessScriptData(ScriptData *scriptData);
  static bool GetSettingString(RED4ext::CName, RED4ext::CName,
                               RED4ext::CString *);
  static void ClearVariables();
  static void AddVariable(IModSettingsVariable *);
  static void WriteToFile();
  static void ReadFromFile();

  static void RegisterTypes();
  static void PostRegisterTypes();

  void NotifyListeners();

  bool changeMade = false;
  RED4ext::DynArray<IModSettingsVariable *> variables;
  RED4ext::DynArray<RED4ext::CName> mods;
  RED4ext::DynArray<RED4ext::CName> classes;
  RED4ext::HashMap<RED4ext::CName, RED4ext::DynArray<IModSettingsVariable *>>
      variablesByMod;
  RED4ext::HashMap<RED4ext::CName, RED4ext::DynArray<IModSettingsVariable *>>
      variablesByClass;
  RED4ext::HashMap<RED4ext::CName, RED4ext::DynArray<RED4ext::CName>>
      categoriesByMod;
  RED4ext::SharedMutex variables_lock;
  RED4ext::DynArray<RED4ext::IScriptable *> listeners;
  RED4ext::SharedMutex listeners_lock;
};