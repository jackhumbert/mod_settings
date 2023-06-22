#include "RuntimeVariable.hpp"
#include "RED4ext/CName.hpp"
#include "RED4ext/CNamePool.hpp"
#include "RED4ext/Scripting/IScriptable.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "ScriptDefinitions/ScriptProperty.hpp"
#include "Variable.hpp"
#include "ModConfigVar.hpp"
#include <algorithm>

namespace ModSettings {

const CName ToConfigVar(CName typeName) noexcept {
  switch (typeName) {
    case CName("Bool"):
      return "ModConfigVarBool";
    case CName("Int32"):
      return "ModConfigVarInt32";
    case CName("Float"):
      return "ModConfigVarFloat";
    default: 
      return "ModConfigVarEnum";
  }
}

// Variable

ModVariable& ModCategory::AddVariable(ModVariable &variable) {
  this->variables[variable.name] = variable;
  variable.category = this;
  return this->variables[variable.name];
}

ModVariable& ModClass::AddVariable(ModVariable &variable, ModCategory &category) {
  if (!this->categories.contains(category.name)) {
    category.modClass = this;
    this->categories[category.name] = category;
  }
  return this->categories[category.name].AddVariable(variable);
}

ModVariable& Mod::AddVariable(ModVariable &variable, ModCategory &category, ModClass &modClass) {
  if (!this->classes.contains(modClass.name)) {
    modClass.mod = this;
    this->classes[modClass.name] = modClass;
  }
  return this->classes[modClass.name].AddVariable(variable, category);
}

bool ModVariable::RestoreDefault() {
  this->runtimeVar->RestoreDefault(0);
  return this->runtimeVar->HasChange();
}

void ModVariable::RejectChange() {
  if (this->runtimeVar->HasChange()) {
    this->runtimeVar->RevertChange();
  }
}

bool ModVariable::IsEnabled() const {
  if (this->dependency.variable) {
    return this->dependency.variable->IsInputEqualToString(this->dependency.value);
  }
  return true;
}

bool ModVariable::IsInputEqualToString(const CString& other) const {
  CString str;
  this->type->ToString(this->runtimeVar->GetInputValue(), str);
  return str == other;
}

void ModVariable::Write(std::ofstream& stream) const {
  this->runtimeVar->ApplyChange();
  if (this->runtimeVar->WasModifiedSinceLastSave()) {
    this->runtimeVar->ChangeWasWritten();
  }
  auto str = RED4ext::CString(new RED4ext::Memory::DefaultAllocator());
  this->type->ToString(this->runtimeVar->GetValuePtr(), str);

  stream << this->runtimeVar->name.ToString() << " = " << str.c_str() << "\n";
}

bool ModVariable::SetRuntimeVariable(ScriptProperty * prop) {
  if (!this->type) {
    return false;
  }
  switch (this->type->GetName()) {
    case CName("Bool"):
      this->runtimeVar = new RuntimeVariableBool(prop);
      return true;
    case CName("Int32"):
      this->runtimeVar = new RuntimeVariableRange<int32_t>(prop);
      return true;
    case CName("Uint32"):
      this->runtimeVar = new RuntimeVariableRange<uint32_t>(prop);
      return true;
    case CName("Float"):
      this->runtimeVar = new RuntimeVariableRange<float>(prop);
      return true;
    default: 
      if(this->type->GetType() == RED4ext::ERTTIType::Enum) {
        this->runtimeVar = new RuntimeVariableEnum(prop);
        return true;
      } else {
        this->runtimeVar = nullptr;
        return false;
      }
  }
}

bool ModVariable::CreateRuntimeVariable(const Variable &var) {
  switch (var.type) {
    case CName("Bool"):
      this->runtimeVar = new RuntimeVariableBool(var.className, var.propertyName, CNamePool::Add(var.displayName), CNamePool::Add(var.description), var.order, var.defaultValue.b);
      return true;
    case CName("Int32"):
      this->runtimeVar = new RuntimeVariableRange<int32_t>(var.className, var.propertyName, CNamePool::Add(var.displayName), CNamePool::Add(var.description), var.order, var.defaultValue.i32, var.stepValue.i32, var.minValue.i32, var.maxValue.i32);
      return true;
    case CName("Uint32"):
      this->runtimeVar = new RuntimeVariableRange<uint32_t>(var.className, var.propertyName, CNamePool::Add(var.displayName), CNamePool::Add(var.description), var.order, var.defaultValue.u32, var.stepValue.u32, var.minValue.u32, var.maxValue.u32);
      return true;
    case CName("Float"):
      this->runtimeVar = new RuntimeVariableRange<float>(var.className, var.propertyName, CNamePool::Add(var.displayName), CNamePool::Add(var.description), var.order, var.defaultValue.f32, var.stepValue.f32, var.minValue.f32, var.maxValue.f32);
      return true;
    default: 
      // if(this->type->GetType() == RED4ext::ERTTIType::Enum) {
      //   this->runtimeVar = new RuntimeVariableEnum(var.className, var.propertyName, var.displayName, var.description, var.order, *var.defaultValue);
      //   return true;
      // } else {
        this->runtimeVar = nullptr;
        return false;
      // }
  }
}

IModConfigVar * ModVariable::ToConfigVar() const {
  if (this->runtimeVar) {
    auto configVar = this->configVarType->CreateInstance<IModConfigVar*>();
    configVar->SetRuntime(this->runtimeVar);
    return configVar;
  } else {
    return nullptr;
  }
}

// Class

// ModClass::ModClass(CName name) : ModClass() {
//   this->name = name;
//   this->type = ToClass(name);
// }

void ModClass::RegisterListener(Handle<IScriptable> &listener) {
  if (listener) {
    this->listeners.emplace_back(listener->ref);
  }
}

void ModClass::UnregisterListener(Handle<IScriptable> &listener) {
  if (listener) {
    auto position = std::find(this->listeners.begin(), this->listeners.end(), listener->ref);
    if (position != this->listeners.end())
      this->listeners.erase(position);
  }
}

void ModClass::RegisterCallback(std::shared_ptr<runtime_class_callback_t> &callback) {
  if (callback) {
    this->callbacks.emplace_back(callback);
  }
}

void ModClass::UpdateDefault(CName propertyName, ScriptInstance* value) const {
  if (this->type) {
      for (auto i = 0; i < this->type->defaults.keys.size; i++) {
        if (this->type->defaults.keys[i] == propertyName) {
          // sdk->logger->InfoF(pluginHandle, "Loaded %s.%s", this->name.ToString(), propertyName.ToString());
          auto propType = this->type->defaults.values[i]->GetType();
          this->type->defaults.values[i]->Fill(propType, value);
        }
      }
  }
}

void ModClass::NotifyListeners() const {
  if (this->type) {
    for (const auto &[categoryName, category] : this->categories) {
      for (const auto &[variableName, variable] : category.variables) {
        auto valuePtr = variable.runtimeVar->GetValuePtr();
        this->UpdateDefault(variableName, valuePtr);
        // std::shared_lock<RED4ext::SharedMutex> _(listeners_lock);
        for (auto &listener : this->listeners) {
          if (listener) {
            auto prop = this->type->propsByName.Get(variable.name);
            if (prop && *prop) {
              (*prop)->SetValue(listener.instance, valuePtr);
            }
          }
        }
      }
    }
  }
  // notify runtime listeners
  for (const auto &[categoryName, category] : this->categories) {
    for (const auto &[variableName, variable] : category.variables) {
      auto valuePtr = variable.runtimeVar->GetValuePtr();
      for (auto &callback : this->callbacks) {
        if (callback) {
          (*callback)(categoryName, variableName, *(ModVariableType*)valuePtr);
        }
      }
    }
  }
}

// Mod

Mod::Mod(CName name) : name(name) {
  // test
  // ModVariable var = {
  //   .name = "My Prop",
  //   .type = ToType("Bool"),
  //   .configVarType = ToClass(ToConfigVar("Bool"))
  // };
  // this->AddVariable(var, CName(), name);
}

// void ModSettingDependency::Resolve(std::string str, CName scriptClass) {
//   trim(str);
//   if (!str.empty()) {
//     auto period = str.find(".");
//     auto equals = str.find("=");
//     if (equals != std::string::npos) {
//       auto value = str.substr(equals, str.length());
//       trim(value);
//       if (value.length()) {
//         this->value = value;
//       }
//     } else {
//       equals = str.length();
//       this->value = "true";
//     }
//     if (period != std::string::npos) {
//       auto className = str.substr(0, period);
//       auto propertyName = str.substr(period+1, equals);
//       trim(className);
//       trim(propertyName);
//       this->propertyName = RED4ext::CNamePool::Add(propertyName.c_str());
//       this->className = RED4ext::CNamePool::Add(className.c_str());
//     } else {
//       this->className = scriptClass;
//       this->propertyName = RED4ext::CNamePool::Add(str.c_str());
//     }
//   }


/*

ModSettingsVariable::ModSettingsVariable() {
  this->listeners = RED4ext::DynArray<RED4ext::WeakHandle<RED4ext::IScriptable>>(new RED4ext::Memory::DefaultAllocator());
  this->listeners.Reserve(1000);
}

// ModSettingsVariable::ModSettingsVariable(ScriptProperty* prop, const CName scriptClass) : ModSettingsVariable() {
//   prop->ReadProperty("ModSettings.mod", &this->mod);
//   prop->ReadProperty("ModSettings.category", &this->category);
//   this->type = RED4ext::CRTTISystem::Get()->GetType(prop->type->name);
//   this->configVarType = RED4ext::CRTTISystem::Get()->GetClass(ToConfigVar(prop->type->name));
//   this->SetRuntimeSetting(prop);
//   auto depends = prop->ReadProperty<std::string>("ModSettings.dependency");
//   trim(depends);
//   if (!depends.empty()) {
//     auto period = depends.find(".");
//     auto equals = depends.find("=");
//     if (equals != std::string::npos) {
//       auto value = depends.substr(equals, depends.length());
//       trim(value);
//       if (value.length()) {
//         this->value = value;
//       }
//     } else {
//       equals = depends.length();
//       this->dependency.value = "true";
//     }
//     if (period != std::string::npos) {
//       auto className = depends.substr(0, period);
//       auto propertyName = depends.substr(period+1, equals);
//       trim(className);
//       trim(propertyName);
//       this->dependency.propertyName = RED4ext::CNamePool::Add(propertyName.c_str());
//       this->dependency.className = RED4ext::CNamePool::Add(className.c_str());
//     } else {
//       this->dependency.className = scriptClass;
//       this->dependency.propertyName = RED4ext::CNamePool::Add(depends.c_str());
//     }
//   }
// }

// ModSettingsVariable::ModSettingsVariable(const CName mod, const CName typeName, const CName className, const CName category) : 
//     mod(mod), 
//     type(RED4ext::CRTTISystem::Get()->GetType(typeName)), 
//     configVarType(RED4ext::CRTTISystem::Get()->GetClass(ToConfigVar(typeName))),
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
    auto valuePtr = runtimeVar->GetValuePtr();
    for (auto i = 0; i < classType->defaults.keys.size; i++) {
      if (classType->defaults.keys[i] == runtimeVar->name) {
        sdk->logger->InfoF(pluginHandle, "Loaded %s.%s", className.ToString(), runtimeVar->name.ToString());
        auto propType = classType->defaults.values[i]->GetType();
        classType->defaults.values[i]->Fill(propType, valuePtr);
      }
    }
    // std::shared_lock<RED4ext::SharedMutex> _(listeners_lock);
    for (auto &listener : listeners) {
      if (listener) {
        auto prop = classType->propsByName.Get(runtimeVar->name);
        if (prop && *prop) {
          (*prop)->SetValue(listener.instance, valuePtr);
        }
      }
    }
  }
}

Handle<RED4ext::user::SettingsVar> ModSettingsVariable::CreateConfigVar() {
  auto configVar = this->configVarType->CreateInstance<RED4ext::user::SettingsVar*>();
  configVar->runtimeVar = this->runtimeVar;
  auto handle = Handle(configVar);
  handle.refCount->IncRef();
  return handle;
}

void ModSettingsVariable::Write(std::ofstream& stream) {
  this->runtimeVar->ApplyChange();
  if (this->runtimeVar->WasModifiedSinceLastSave()) {
    this->UpdateValues();
    this->runtimeVar->ChangeWasWritten();
  }
  auto str = RED4ext::CString(new RED4ext::Memory::DefaultAllocator());
  this->type->ToString(this->runtimeVar->GetValuePtr(), str);

  stream << this->runtimeVar->name.ToString() << " = " << str.c_str() << "\n";
}

bool ModSettingsVariable::RestoreDefault() {
  this->runtimeVar->RestoreDefault(0);
  return this->runtimeVar->HasChange();
}

void ModSettingsVariable::RejectChange() {
  if (this->runtimeVar->HasChange()) {
    this->runtimeVar->RevertChange();
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

bool ModVariable::SetRuntimeVariable(ScriptProperty * prop) {
  switch (this->type->GetName()) {
    case CName("Bool"):
      this->runtimeVar = new RuntimeVariableBool(prop);
      return true;
    case CName("Int32"):
      this->runtimeVar = new RuntimeVariableRange<int32_t>(prop);
      return true;
    case CName("Uint32"):
      this->runtimeVar = new RuntimeVariableRange<uint32_t>(prop);
      return true;
    case CName("Float"):
      this->runtimeVar = new RuntimeVariableRange<float>(prop);
      return true;
    default: 
      if(this->type->GetType() == RED4ext::ERTTIType::Enum) {
        this->runtimeVar = new RuntimeVariableEnum(prop);
        return true;
      } else {
        this->runtimeVar = nullptr;
        return false;
      }
  }
}
*/

} // namespace ModSettings