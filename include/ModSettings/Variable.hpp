#pragma once

#include "IRuntimeVariable.hpp"
#include "RED4ext/RTTITypes.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>
#include <RED4ext/Scripting/Natives/userSettingsVar.hpp>
#include <map>

namespace ModSettings {

using namespace RED4ext;

struct ScriptProperty;

struct ModSettingDependency {
  static ModSettingDependency* FromString(std::string str, CName scriptClass);

  CName className;
  CName propertyName;
  CString value;
};

struct Mod;
struct ModClass;
struct ModCategory;

struct ModVariable {
  uint32_t GetOrder() const {
    return runtimeVar->order;
  }

  bool operator< (const ModVariable &other) const {
    return GetOrder() < other.GetOrder();
  }

  std::ofstream& operator<< (std::ofstream& stream) {
    this->Write(stream);
    return stream;
  }

  void Write(std::ofstream& stream);
  bool SetRuntimeVariable(ScriptProperty * prop);
  bool RestoreDefault();
  void RejectChange();
  
  CName name = 0LLU;
  CBaseRTTIType *type = nullptr;
  CClass *configVarType = nullptr;
  IRuntimeVariable *runtimeVar = nullptr;
  ModSettingDependency dependency;
  ModCategory * category;
};

// maybe shouldn't use this structure interally?
struct ModCategory {
  ModCategory() = default;
  ModCategory(CName name);

  ModVariable& AddVariable(ModVariable variable);

  constexpr operator CName() const noexcept {
    return this->name;
  }

  CName name;
  uint32_t order;
  std::map<CName, ModVariable> variables;
  ModClass * modClass;
};

struct ModClass {
  ModClass() = default;
  ModClass(CName name);

  ModVariable& AddVariable(ModVariable variable, ModCategory category = CName());
  void RegisterListener(Handle<IScriptable> listener);
  void UnregisterListener(Handle<IScriptable> listener);
  void NotifyListeners();

  constexpr operator CName() const noexcept {
    return this->name;
  }

  CName name;
  uint32_t order;
  CClass* type;
  std::map<uint32_t, WeakHandle<IScriptable>> listeners;
  std::map<CName, ModCategory> categories;
  Mod * mod;
};

struct Mod {
  Mod() = default;
  Mod(CName name);

  ModVariable& AddVariable(ModVariable variable, ModCategory category = CName(), ModClass modClass = CName());

  CName name;
  std::map<CName, ModClass> classes;
};

class ModSettingsVariable {
public:
  ModSettingsVariable();
  ModSettingsVariable(ScriptProperty* prop, const CName scriptClass);
  
  void UpdateValues();
  Handle<user::SettingsVar> CreateConfigVar();
  void Write(std::ofstream& stream);

  inline const CName& Mod() const { return mod; }
  inline const CName& ClassName() const { return className; }
  inline const CName& Category() const { return category; }

  bool RestoreDefault();
  void RejectChange();

  void RegisterListener(Handle<IScriptable> handle);
  void UnregisterListener(Handle<IScriptable> handle);

  inline const bool IsValid() const { return this->runtimeVar != nullptr; }
  
  std::ofstream& operator<< (std::ofstream& stream) {
    this->Write(stream);
    return stream;
  }

  bool SetRuntimeVariable(ScriptProperty * prop);

private:
  template<typename VariableType>
  constexpr const CName GetTypeCName() noexcept;

  CName mod = CName();
  CName className = CName();
  CName category = CName();

  CBaseRTTIType *type;
  CClass *configVarType;
  IRuntimeVariable *runtimeVar = nullptr;
  DynArray<WeakHandle<IScriptable>> listeners;
  // std::mutex listeners_lock;
  ModSettingDependency dependency;
};

}