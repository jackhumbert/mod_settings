#include "ModRuntimeSettingsVar.hpp"
#include "RED4ext/CName.hpp"
#include "RED4ext/CNamePool.hpp"
#include "RED4ext/Scripting/IScriptable.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "ScriptDefinitions/ScriptProperty.hpp"
#include <ModSettingsVariable.hpp>

constexpr const CName GetConfigVarTypeCName(CName typeName) noexcept {
  switch (typeName) {
    case CName("Bool"):
      return "ModConfigVarBool";
    case CName("Int32"):
      return "ModConfigVarInt32";
    case CName("Float"):
      return "ModConfigVarFloat";
    case CName("Enum"):
      return "ModConfigVarEnum";
    default: 
      return 0LLU;
  }
}

void trim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

ModSettingsVariable::ModSettingsVariable() {
  this->listeners = RED4ext::DynArray<RED4ext::WeakHandle<RED4ext::IScriptable>>(new RED4ext::Memory::DefaultAllocator());
  this->listeners.Reserve(1000);
}

ModSettingsVariable::ModSettingsVariable(ScriptProperty* prop, const CName scriptClass) : ModSettingsVariable() {
  prop->ReadProperty("ModSettings.mod", &this->mod);
  prop->ReadProperty("ModSettings.category", &this->category);
  this->type = RED4ext::CRTTISystem::Get()->GetType(prop->type->name);
  this->configVarType = RED4ext::CRTTISystem::Get()->GetClass(GetConfigVarTypeCName(prop->type->name));
  this->SetRuntimeSetting(prop);
  auto depends = prop->ReadPropertyString("ModSettings.dependency");
  trim(depends);
  if (!depends.empty()) {
    auto period = depends.find(".");
    auto equals = depends.find("=");
    if (equals != std::string::npos) {
      auto value = depends.substr(equals, depends.length());
      trim(value);
      if (value.length()) {
        this->dependency.value = value;
      }
    } else {
      equals = depends.length();
      this->dependency.value = "true";
    }
    if (period != std::string::npos) {
      auto className = depends.substr(0, period);
      auto propertyName = depends.substr(period+1, equals);
      trim(className);
      trim(propertyName);
      this->dependency.propertyName = RED4ext::CNamePool::Add(propertyName.c_str());
      this->dependency.className = RED4ext::CNamePool::Add(className.c_str());
    } else {
      this->dependency.className = scriptClass;
      this->dependency.propertyName = RED4ext::CNamePool::Add(depends.c_str());
    }
  }
}

// ModSettingsVariable::ModSettingsVariable(const CName mod, const CName typeName, const CName className, const CName category) : 
//     mod(mod), 
//     type(RED4ext::CRTTISystem::Get()->GetType(typeName)), 
//     configVarType(RED4ext::CRTTISystem::Get()->GetClass(GetConfigVarTypeCName(typeName))),
//     className(className),
//     category(category) {
//   this->listeners = RED4ext::DynArray<RED4ext::WeakHandle<RED4ext::IScriptable>>(new RED4ext::Memory::DefaultAllocator());
//   this->listeners.Reserve(1000);
// }

template <> constexpr const CName ModSettingsVariable::GetTypeCName<uint32_t>() noexcept {
  return 0x3D2E9DD9E3C28D8C;
}

template <> constexpr const CName ModSettingsVariable::GetTypeCName<bool>() noexcept {
  return 0xF7BDD5A7C820889D;
}

template <> constexpr const CName ModSettingsVariable::GetTypeCName<int32_t>() noexcept {
  return 0xB9A127F5B4A621BF;
}

template <> constexpr const CName ModSettingsVariable::GetTypeCName<CName>() noexcept {
  return 0xA5E23DE2A2657AF9;
}

template <> constexpr const CName ModSettingsVariable::GetTypeCName<float>() noexcept {
  return 0xB64F4A0ACCC8A8C5;
}

void ModSettingsVariable::UpdateValues() {
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
    // std::shared_lock<RED4ext::SharedMutex> _(listeners_lock);
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

Handle<RED4ext::user::SettingsVar> ModSettingsVariable::CreateConfigVar() {
  auto configVar = this->configVarType->CreateInstance<RED4ext::user::SettingsVar*>();
  configVar->runtimeVar = this->settingsVar;
  auto handle = Handle(configVar);
  handle.refCount->IncRef();
  return handle;
}

void ModSettingsVariable::Write(std::ofstream& stream) {
  this->settingsVar->ApplyChange();
  if (this->settingsVar->WasModifiedSinceLastSave()) {
    this->UpdateValues();
    this->settingsVar->ChangeWasWritten();
  }
  auto str = RED4ext::CString(new RED4ext::Memory::DefaultAllocator());
  this->type->ToString(this->settingsVar->GetValuePtr(), str);

  stream << this->settingsVar->name.ToString() << " = " << str.c_str() << "\n";
}

bool ModSettingsVariable::RestoreDefault() {
  this->settingsVar->RestoreDefault(0);
  return this->settingsVar->HasChange();
}

void ModSettingsVariable::RejectChange() {
  if (this->settingsVar->HasChange()) {
    this->settingsVar->RevertChange();
  }
}

void ModSettingsVariable::RegisterListener(Handle<RED4ext::IScriptable> handle) {
  // std::shared_lock<RED4ext::SharedMutex> _(this->listeners_lock);
  this->listeners.EmplaceBack(handle);
}

void ModSettingsVariable::UnregisterListener(Handle<RED4ext::IScriptable> handle) {
  auto i = 0;
  for (const auto &listener : this->listeners) {
    // std::shared_lock<RED4ext::SharedMutex> _(this->listeners_lock);
    if (listener && listener.instance == handle.instance) {
      this->listeners.RemoveAt(i);
      break;
    }
    i++;
  }
}

bool ModSettingsVariable::SetRuntimeSetting(ScriptProperty * prop) {
  switch (this->type->GetName()) {
    case CName("Bool"):
      this->settingsVar = new ModRuntimeSettingsVarBool(prop);
      return true;
    case CName("Int32"):
      this->settingsVar = new ModRuntimeSettingsVarRange<int32_t>(prop);
      return true;
    case CName("Uint32"):
      this->settingsVar = new ModRuntimeSettingsVarRange<uint32_t>(prop);
      return true;
    case CName("Float"):
      this->settingsVar = new ModRuntimeSettingsVarRange<float>(prop);
      return true;
    default: 
      if(this->type->GetType() == RED4ext::ERTTIType::Enum) {
        this->settingsVar = new ModRuntimeSettingsVarEnum(prop);
        return true;
      } else {
        this->settingsVar = nullptr;
        return false;
      }
  }
}