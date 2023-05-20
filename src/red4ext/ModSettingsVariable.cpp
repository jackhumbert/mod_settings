#include "ModRuntimeSettingsVar.hpp"
#include "RED4ext/Scripting/IScriptable.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include <ModSettingsVariable.hpp>

template <> constexpr const RED4ext::CName ModSettingsVariable::GetTypeCName<uint32_t>() noexcept {
  return 0x3D2E9DD9E3C28D8C;
}

template <> constexpr const RED4ext::CName ModSettingsVariable::GetTypeCName<bool>() noexcept {
  return 0xF7BDD5A7C820889D;
}

template <> constexpr const RED4ext::CName ModSettingsVariable::GetTypeCName<int32_t>() noexcept {
  return 0xB9A127F5B4A621BF;
}

template <> constexpr const RED4ext::CName ModSettingsVariable::GetTypeCName<RED4ext::CName>() noexcept {
  return 0xA5E23DE2A2657AF9;
}

template <> constexpr const RED4ext::CName ModSettingsVariable::GetTypeCName<float>() noexcept {
  return 0xB64F4A0ACCC8A8C5;
}

// template <typename Type, RED4ext::CName Name>
// ModSettingsVariable<Type, Name>::ModSettingsVariable(const RED4ext::CName mod, const RED4ext::CName className,
//                                                const RED4ext::CName category) {
//   this->typeName = GetTypeCName();
//   this->mod = mod;
//   this->className = className;
//   this->category = category;
//   this->listeners = RED4ext::DynArray<RED4ext::IScriptable *>(new RED4ext::Memory::DefaultAllocator());
// }

// template <typename Type, RED4ext::CName Name>
// ModSettingsVariable<Type, Name>::ModSettingsVariable(const char *mod, const char *className, const char *category)
//     : ModSettingsVariable(RED4ext::CNamePool::Add(mod), RED4ext::CNamePool::Add(className),
//                           RED4ext::CNamePool::Add(category)) {}

void IModSettingsVariable::UpdateValues() {
  auto classType = RED4ext::CRTTISystem::Get()->GetClass(className);
  if (classType) {
    auto valuePtr = settingsVar->GetValuePtr();
    for (auto i = 0; i < classType->defaults.keys.size; i++) {
      if (classType->defaults.keys[i] == settingsVar->name) {
        sdk->logger->InfoF(pluginHandle, "Loaded %s.%s", className.ToString(), settingsVar->name.ToString());
        auto propType = classType->defaults.values[i]->GetType();
        classType->defaults.values[i]->Fill(propType, valuePtr);
      }
    }
    std::shared_lock<RED4ext::SharedMutex> _(listeners_lock);
    for (auto &listener : listeners) {
      if (listener) {
        auto prop = classType->propsByName.Get(settingsVar->name);
        if (prop && *prop) {
          (*prop)->SetValue(listener.instance, valuePtr);
        }
      }
    }
  }
}