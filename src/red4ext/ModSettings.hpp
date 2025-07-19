#pragma once

#include "Utils.hpp"
#include "Variable.hpp"
#include <RED4ext/Common.hpp>

struct Manager;

namespace ModSettings {

extern const Sdk *sdk;
extern PluginHandle pluginHandle;

struct ScriptData;

class ModSettings : public IScriptable {
public:
  ModSettings();

  virtual CClass *GetNativeType() override;
  virtual Memory::IAllocator *GetAllocator() override;
  virtual bool CanBeDestructed() override {
    return false;
  }

  static Handle<ModSettings> GetInstance();

  static void ProcessScriptData(const ScriptData *scriptData);
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
  static void RegisterListenerToModifications(const Handle<IScriptable>& listener);
  static void UnregisterListenerToModifications(const Handle<IScriptable>& listener);
  static void RegisterListenerToClass(const Handle<IScriptable>& listener);
  static void UnregisterListenerToClass(const Handle<IScriptable>& listener);

  static DynArray<CName> GetMods();
  static DynArray<CName> GetCategories(CName modName);
  static DynArray<Handle<IScriptable>> GetVars(CName modName, CName categoryName);
  static void AddOverrides(Manager* manager);


  void NotifyListeners();
  void NotifyListenersRequested(CName, CName);
  void NotifyListenersAccepted(CName, CName);

  bool changesRequested = false;

  static_assert(Detail::IsAllocator<Memory::RTTIAllocator>,
                "Memory::RTTIAllocator"
                " is not a valid allocator type");
private:
  // std::shared_mutex variables_lock;
  RED4ext::SharedSpinLock variables_lock;
  std::map<uint32_t, WeakHandle<IScriptable>> listeners;
  // std::shared_mutex listeners_lock;
  RED4ext::SharedSpinLock listeners_lock;
  std::map<CName, Mod*> mods;
};

} // namespace ModSettings