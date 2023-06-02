#pragma once

#include <ModSettings/Utils.hpp>
#include <ModSettings/Variable.hpp>
#include <RED4ext/Common.hpp>

#ifdef MOD_SETTINGS_DLLDIR_EX
#define MOD_SETTINGS_DLLDIR __declspec(dllexport) // export DLL information
#else
#define MOD_SETTINGS_DLLDIR __declspec(dllimport) // import DLL information
#endif

namespace ModSettings {

extern const Sdk *sdk;
extern PluginHandle pluginHandle;

struct ScriptData;

class MOD_SETTINGS_DLLDIR ModSettings : public IScriptable {
public:
  ModSettings();

  virtual CClass *GetNativeType() override;
  virtual Memory::IAllocator *GetAllocator() override;
  virtual bool CanBeDestructed() override {
    return false;
  }

  static Handle<ModSettings> GetInstance();

  static void ProcessScriptData(ScriptData *scriptData);
  static bool GetSettingString(CName, CName, CString *);
  static void ReadValueFromFile(ScriptProperty *prop, ScriptInstance pointer);

  template <typename T>
  static void ReadValueFromFile(CName className, CName propertyName,
                                T *pointer) {
    auto self = ModSettings::GetInstance();
    CString settingFromFile;
    if (ModSettings::GetSettingString(className, propertyName,
                                      &settingFromFile)) {
      RedTypeFromString(pointer, settingFromFile);
    }
  }

  static void ClearVariables();
  // static void AddVariable(ModSettingsVariable *);
  static void WriteToFile();
  static void ReadFromFile();
  static void AcceptChanges();
  static void RestoreDefaults(CName modName);
  static void RejectChanges();
  static void RegisterListenerToModifications(Handle<IScriptable>& listener);
  static void UnregisterListenerToModifications(Handle<IScriptable>& listener);
  static void RegisterListenerToClass(Handle<IScriptable>& listener);
  static void UnregisterListenerToClass(Handle<IScriptable>& listener);

  static DynArray<CName> GetMods();
  static DynArray<CName> GetCategories(CName modName);
  static DynArray<Handle<IScriptable>> GetVars(CName modName, CName categoryName);

  void NotifyListeners();

  bool changeMade = false;

  static_assert(Detail::IsAllocator<Memory::RTTIAllocator>,
                "Memory::RTTIAllocator"
                " is not a valid allocator type");
private:
  SharedMutex variables_lock;
  std::map<uint32_t, WeakHandle<IScriptable>> listeners;
  SharedMutex listeners_lock;
  std::map<CName, Mod> mods;
};

} // namespace ModSettings