#pragma once

#include <ModSettings/ModSettings.hpp>
#include "IRuntimeVariable.hpp"
#include "RED4ext/RTTITypes.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>
#include <RED4ext/Scripting/Natives/userSettingsVar.hpp>
#include <map>
#include <functional>

struct IModConfigVar;

namespace ModSettings {

using namespace RED4ext;

struct ScriptProperty;

struct Mod;
struct ModClass;
struct ModCategory;
struct ModVariable;

const CName ToConfigVar(CName typeName) noexcept;

struct ModVariable {
  uint32_t GetOrder() const {
    return runtimeVar->order != (uint32_t)-1 ? runtimeVar->order : implicitOrder;
  }

  bool operator< (const ModVariable &other) const {
    return GetOrder() < other.GetOrder();
  }

  friend std::ofstream& operator<< (std::ofstream& stream, const ModVariable& mv) {
    mv.Write(stream);
    return stream;
  }

  void Write(std::ofstream& stream) const;
  bool SetRuntimeVariable(ScriptProperty * prop);
  bool CreateRuntimeVariable(const Variable &var);
  bool RestoreDefault();
  void RejectChange();
  bool IsEnabled() const;
  bool IsInputEqualToString(const CString& str) const;
  IModConfigVar * ToConfigVar() const;
  
  CName name = 0LLU;
  CBaseRTTIType *type = nullptr;
  CClass *configVarType = nullptr;
  IRuntimeVariable *runtimeVar = nullptr;
  ModSettingDependency dependency;
  ModCategory * category;
  uint32_t implicitOrder;
};

// maybe shouldn't use this structure interally?
struct ModCategory {
  // ModCategory() = default;
  // ModCategory(CName name);

  ModVariable& AddVariable(ModVariable &variable);

  constexpr operator CName() const noexcept {
    return this->name;
  }

  CName name;
  uint32_t order;
  std::map<CName, ModVariable> variables;
  ModClass * modClass;
};

struct ModClass {
  // ModClass() = default;
  // ModClass(CName, CClass*, Mod*);

  // ModClass(const ModClass&) = default;
  // ModClass &operator=(const ModClass &) = default;
  // ModClass(CName name);

  ModVariable& AddVariable(ModVariable &variable, ModCategory &category);
  void RegisterListener(const Handle<IScriptable> &listener);
  void UnregisterListener(const Handle<IScriptable> &listener);
  void RegisterCallback(std::shared_ptr<runtime_class_callback_t> &callback);
  void UpdateDefault(CName propertyName, ScriptInstance* value) const;
  void NotifyListeners() const;

  constexpr operator CName() const noexcept {
    return this->name;
  }

  CName name;
  uint32_t order;
  CClass* type;
  std::shared_mutex * listeners_lock = new std::shared_mutex();
  std::vector<WeakHandle<ISerializable>> listeners;
  std::shared_mutex * callbacks_lock = new std::shared_mutex();
  std::vector<std::shared_ptr<runtime_class_callback_t>> callbacks;
  std::map<CName, ModCategory> categories;
  Mod * mod;
};

struct Mod {
  Mod() = default;
  Mod(CName name);

  ModVariable& AddVariable(ModVariable &variable, ModCategory &category, ModClass &modClass);

  CName name;
  std::shared_mutex * classes_lock = new std::shared_mutex();
  std::map<CName, ModClass> classes;
};
/*
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
*/
}