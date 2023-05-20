#pragma once

#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

struct IModRuntimeSettingsVar;

struct IModSettingsVariable {
  RED4ext::CName mod = RED4ext::CName();
  RED4ext::CName typeName = RED4ext::CName();
  RED4ext::CName className = RED4ext::CName();
  RED4ext::CName category = RED4ext::CName();
  IModRuntimeSettingsVar *settingsVar;
  RED4ext::DynArray<RED4ext::WeakHandle<RED4ext::IScriptable>> listeners;
  RED4ext::SharedMutex listeners_lock;

  void UpdateValues();
};

struct ModSettingsVariable : IModSettingsVariable {
  inline ModSettingsVariable(const RED4ext::CName aMod, const RED4ext::CName typeName, const RED4ext::CName className, const RED4ext::CName category) {
    this->mod = mod;
    this->typeName = typeName;
    this->className = className;
    this->category = category;
    this->settingsVar = nullptr;
    this->listeners = RED4ext::DynArray<RED4ext::WeakHandle<RED4ext::IScriptable>>(new RED4ext::Memory::DefaultAllocator());
    this->listeners.Reserve(1000);
  }

  template<typename VariableType, class ClassName = RED4ext::IScriptable>
  inline ModSettingsVariable(const RED4ext::CName mod, const ClassName listener, const RED4ext::CName category, VariableType defaultValue) {
    static_assert(std::is_convertible<ClassName*, RED4ext::IScriptable*>::value, "listener must inherit IScriptable");
    ModSettingsVariable(mod, GetTypeCName<VariableType>(), listener->GetNative()->GetName(), category);
  }

  template<typename VariableType>
  inline ModSettingsVariable(const RED4ext::CName mod, const RED4ext::CName className, const RED4ext::CName category, VariableType defaultValue) :
    ModSettingsVariable(mod, GetTypeCName<VariableType>(), className, category) {
  }


  template<typename VariableType>
  constexpr const RED4ext::CName GetTypeCName() noexcept;
};