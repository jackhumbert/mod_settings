#include "INIReader.h"
#include "ModConfigVar.hpp"
#include "Red/TypeInfo/Macros/Definition.hpp"
#include "RuntimeVariable.hpp"
#include "ScriptData.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "Scripting/RTTIRegistrar.hpp"
#include "Utils.hpp"
#include "stdafx.hpp"
#include <ModSettings/ModSettings.hpp>
#include "ModSettings.hpp"
#include "Variable.hpp"
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVar.hpp>
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVarBool.hpp>
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVarFloat.hpp>
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVarInt.hpp>
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVarListInt.hpp>
#include <RedLib.hpp>
#include <iostream>


namespace ModSettings {

INIReader reader;

const std::filesystem::path configPath = Utils::GetRootDir() / "red4ext" / "plugins" / "mod_settings" / "user.ini";

ModSettings modSettings = ModSettings();

ModSettings::ModSettings() {}

Handle<ModSettings> ModSettings::GetInstance() { return Handle<ModSettings>(&modSettings); }

void ModSettings::ClearVariables() {
  modSettings.mods.clear();
}

std::vector<Variable*> queuedVariables;

void AddVariable(Variable* variable) {
  queuedVariables.emplace_back(variable);
}

void __fastcall ModSettings::ProcessScriptData(ScriptData *scriptData) {
  auto self = ModSettings::GetInstance();
  if (scriptData) {
    ModSettings::ReadFromFile();
    ModSettings::ClearVariables();
    for (const auto &scriptClass : scriptData->classes) {
      for (const auto &prop : scriptClass->properties) {
        // filter out inherited props?
        // if (scriptClass->parent) {
        //   auto found = false;
        //   for (const auto &parentProp : scriptClass->parent->properties) {
        //     found |= prop->name == parentProp->name;
        //   }
        //   if (found)
        //     continue;
        // }
        if (prop->runtimeProperties.size) {
          if (prop->IsValid()) {
            auto modName = prop->ReadProperty("ModSettings.mod");
            if (!self->mods.contains(modName)) {
              self->mods[modName] = Mod(modName);
            }
            auto &mod = self->mods[modName];

            if (!mod.classes.contains(scriptClass->name)) {
              mod.classes[scriptClass->name] = {
                .name = scriptClass->name,
                .type = ToClass(scriptClass->name),
                .mod = &mod
              };
            }
            auto &modClass = mod.classes[scriptClass->name];

            auto categoryName = prop->ReadProperty("ModSettings.category");
            if (!modClass.categories.contains(categoryName)) {
              modClass.categories[categoryName] = {
                .name = categoryName,
                .order = prop->ReadProperty<uint32_t>("ModSettings.category.order"),
                .modClass = &modClass
              };
            }
            auto &category = modClass.categories[categoryName];

            ModVariable variable = {
              .name = prop->name,
              .type = CRTTISystem::Get()->GetType(prop->type->name),
              .configVarType = CRTTISystem::Get()->GetClass(ToConfigVar(prop->type->name)),
              .dependency = *prop->ReadDependency(scriptClass->name),
              .category = &category,
              .implicitOrder = (uint32_t)category.variables.size()
            };
            
            if (variable.SetRuntimeVariable(prop)) {
              category.variables[prop->name] = variable;
              modClass.UpdateDefault(variable.name, variable.runtimeVar->GetValuePtr());
              sdk->logger->InfoF(pluginHandle, "Loaded %s.%s", modClass.name.ToString(), variable.name.ToString());
            } else {
              sdk->logger->ErrorF(pluginHandle, "%s.%s: Could not find runtime type for script type %s", modClass.name.ToString(), prop->name.ToString(), prop->type->name.ToString());
            }
          }
        }
      }
    }
    for (const auto &var : queuedVariables) {
      CNamePool::Add(var->modName);
      if (!self->mods.contains(var->modName)) {
        self->mods[var->modName] = Mod(var->modName);
      }
      auto &mod = self->mods[var->modName];

      auto modClassName = CNamePool::Add(var->className);
      if (!mod.classes.contains(modClassName)) {
        mod.classes[modClassName] = {
          .name = modClassName,
          .mod = &mod
        };
      }
      auto &modClass = mod.classes[modClassName];

      auto categoryName = CNamePool::Add(var->categoryName);
      if (!modClass.categories.contains(categoryName)) {
        modClass.categories[categoryName] = {
          .name = categoryName,
          .modClass = &modClass
        };
      }
      auto &category = modClass.categories[categoryName];

      auto variableName = CNamePool::Add(var->propertyName);
      ModVariable variable = {
        .name = variableName,
        .type = CRTTISystem::Get()->GetType(var->type),
        .configVarType = CRTTISystem::Get()->GetClass(ToConfigVar(var->type)),
        .dependency = var->dependency,
        .category = &category,
        .implicitOrder = (uint32_t)category.variables.size()
      };

      if (variable.CreateRuntimeVariable(*var)) {
        category.variables[variableName] = variable;
        modClass.RegisterCallback(var->callback);
        (*var->callback)(var->categoryName, var->propertyName, *(ModVariableType*)variable.runtimeVar->GetValuePtr());
        sdk->logger->InfoF(pluginHandle, "Loaded '%s'.%s", var->modName, var->propertyName);
      } else {
        sdk->logger->ErrorF(pluginHandle, "Could not create runtime variable for {}", var->propertyName);
      }
    }
    // resolve dependencies
    for (auto &[_, mod] : self->mods) {
      for (auto &[_, modClass] : mod.classes) {
        for (auto &[_, category] : modClass.categories) {
          for (auto &[_, variable] : category.variables) {
            if (variable.dependency.propertyName) {
              if (mod.classes.contains(variable.dependency.className)) {
                auto &depClass = mod.classes[variable.dependency.className];
                for (auto &[_, depCategory] : depClass.categories) {
                  if (depCategory.variables.contains(variable.dependency.propertyName)) {
                      variable.dependency.variable = &depCategory.variables[variable.dependency.propertyName];
                  } 
                }
              }
            }
          }
        }
      }
    }
  }
}

// void ModSettings::AddVariable(ModSettingsVariable *variable) {
//   auto self = ModSettings::GetInstance();

//   std::shared_lock<SharedMutex> _(self->variables_lock);
//   variable->UpdateValues();
//   self->variables.EmplaceBack(variable);

//   auto modVars = self->variablesByMod.Get(variable->Mod());
//   if (modVars) {
//     modVars->EmplaceBack(variable);
//   } else {
//     auto ra = DynArray<ModSettingsVariable *>(new Memory::DefaultAllocator);
//     ra.EmplaceBack(variable);
//     self->variablesByMod.Insert(variable->Mod(), ra);
//     self->mods.EmplaceBack(variable->Mod());
//   }

//   auto classVars = self->variablesByClass.Get(variable->ClassName());
//   if (classVars) {
//     classVars->EmplaceBack(variable);
//   } else {
//     auto ra = DynArray<ModSettingsVariable *>(new Memory::DefaultAllocator);
//     ra.EmplaceBack(variable);
//     self->variablesByClass.Insert(variable->ClassName(), ra);
//     self->classes.EmplaceBack(variable->ClassName());
//   }

//   if (variable->Category() != "None") {
//     auto modCategories = self->categoriesByMod.Get(variable->Mod());
//     if (modCategories) {
//       auto found = false;
//       for (const auto &category : *modCategories) {
//         found |= (variable->Category() == category);
//       }
//       if (!found) {
//         modCategories->EmplaceBack(variable->Category());
//       }
//     } else {
//       auto ra = DynArray<CName>(new Memory::DefaultAllocator);
//       ra.EmplaceBack(variable->Category());
//       self->categoriesByMod.Insert(variable->Mod(), ra);
//     }
//   }
// }

DynArray<CName> ModSettings::GetMods() {
  auto array = DynArray<CName>(new Memory::DefaultAllocator);
  for (auto const &[modName, mod] : modSettings.mods) {
    array.EmplaceBack(modName);
  }
  return array;
}

DynArray<CName> ModSettings::GetCategories(CName modName) {
  auto array = DynArray<CName>(new Memory::DefaultAllocator);
  for (auto const &[modClassName, modClass] : modSettings.mods[modName].classes) {
    std::vector<std::pair<CName, ModCategory>> modCategories;
    for (auto itr = modClass.categories.begin(); itr != modClass.categories.end(); ++itr ) {
      modCategories.push_back(*itr); 
    }
    sort(modCategories.begin(), modCategories.end(), [=](std::pair<CName, ModCategory>& a, std::pair<CName, ModCategory>& b) {
        return a.second.order < b.second.order;
    });
    for (auto const &[categoryName, category] : modCategories) {
      if (categoryName != "None" && !category.variables.empty()) {
        auto position = std::find(array.begin(), array.end(), categoryName);
        if (position == array.end())
          array.EmplaceBack(categoryName);
      }
    }
  }
  return array;
}

DynArray<Handle<IScriptable>> ModSettings::GetVars(CName modName, CName categoryName) {
  auto array = DynArray<Handle<IScriptable>>(new Memory::DefaultAllocator);
  if (modSettings.mods.contains(modName)) {
    std::vector<ModVariable> variables;
    for (auto &[modClassName, modClass] : modSettings.mods[modName].classes) {
      if (modClass.categories.contains(categoryName)) {
        for (auto const &[variableName, variable] : modClass.categories[categoryName].variables) {
          if (variable.IsEnabled()) {
            variables.emplace_back(variable);
          }
        }
      }
    }
    std::sort(variables.begin(), variables.end());
    for (auto const &variable : variables) {
      auto configVar = variable.ToConfigVar();
      if (configVar) {
        array.EmplaceBack(Handle(configVar));
      }
    }
  }
  return array;
}

void ModSettings::WriteToFile() {
  std::ofstream configFile(configPath);
  if (configFile.is_open()) {
    for (const auto &[modName, mod] : modSettings.mods) {
      for (const auto &[className, modClass] : mod.classes) {
        configFile << "[" << className.ToString() << "]\n";
        for (const auto &[categoryName, category] : modClass.categories) {
          for (const auto &[variableName, variable] : category.variables) {
            configFile << variable;
          }
        }
        modClass.NotifyListeners();
        configFile << "\n";
      }
    }
    configFile.close();
    sdk->logger->InfoF(pluginHandle, "User settings written to file: %s", configPath.string().c_str());
  } else {
    sdk->logger->InfoF(pluginHandle, "Could not write to file: %s", configPath.string().c_str());
  }
}

bool ModSettings::GetSettingString(CName className, CName propertyName, CString *value) {
  std::string defaultValue = "";
  auto valueStr = reader.Get(className.ToString(), propertyName.ToString(), defaultValue);
  if (valueStr != defaultValue) {
    *value = CString(valueStr.c_str());
    return true;
  } else {
    return false;
  }
}

void ModSettings::ReadValueFromFile(ScriptProperty *prop, ScriptInstance pointer) {
  CString settingFromFile;
  if (ModSettings::GetSettingString(prop->parent->name, prop->name, &settingFromFile)) {
    prop->FromString(pointer, settingFromFile);
  }
}

void ModSettings::ReadFromFile() {
  reader = INIReader(configPath.string());

  if (reader.ParseError() != 0) {
    return;
  }
}

void ModSettings::AcceptChanges() {
  ModSettings::WriteToFile();
  modSettings.changeMade = false;
  modSettings.NotifyListeners();
}

void ModSettings::RestoreDefaults(CName modName) {
  if (modSettings.mods.contains(modName)) {
    auto mod = modSettings.mods[modName];
    modSettings.changeMade = false;
    for (auto [modClassName, modClass] : mod.classes) {
      for (auto [categoryName, category] : modClass.categories) {
        for (auto [variableName, variable] : category.variables) {
          modSettings.changeMade |= variable.RestoreDefault();
        }
      }
    }
    modSettings.NotifyListeners();
  }
}

void ModSettings::RejectChanges() {
  for (auto [modName, mod] : modSettings.mods) {
    for (auto [modClassName, modClass] : mod.classes) {
      for (auto [categoryName, category] : modClass.categories) {
        for (auto [variableName, variable] : category.variables) {
          modSettings.changeMade |= variable.RestoreDefault();
        }
      }
    }
  }
  modSettings.changeMade = false;
  modSettings.NotifyListeners();
}

void ModSettings::NotifyListeners() {
  std::shared_lock<SharedMutex> _(listeners_lock);
  for (auto &[id, listener] : this->listeners) {
    if (listener) {
      auto instance = listener.Lock();
      auto func = instance->GetType()->GetFunction("OnModSettingsChange");
      if (func) 
        RED4ext::ExecuteFunction(instance, func, nullptr);
    } else {
      // remove?
    }
  }
}

void ModSettings::RegisterListenerToModifications(Handle<IScriptable> &listener) {
  if (listener) {
    modSettings.listeners[listener->unk28] = listener;
  }
}

void ModSettings::UnregisterListenerToModifications(Handle<IScriptable> &listener) {
  if (listener) {
    modSettings.listeners.erase(listener->unk28);
  }
}

void ModSettings::RegisterListenerToClass(Handle<IScriptable> &listener) {
  if (listener) {
    auto className = listener->GetType()->GetName();
    for (auto &[modName, mod] : modSettings.mods) {
      if (mod.classes.contains(className)) {
        mod.classes[className].RegisterListener(listener);
      }
    }
  }
}

void ModSettings::UnregisterListenerToClass(Handle<IScriptable> &listener) {
  if (listener) {
    auto className = listener->GetType()->GetName();
    for (auto &[modName, mod] : modSettings.mods) {
      if (mod.classes.contains(className)) {
        mod.classes[className].UnregisterListener(listener);
      }
    }
  }
}

// friend struct Red ::TypeInfoBuilder<Red ::Scope ::For<ModSettings>()>;
// friend class Red ::ClassDescriptor<ModSettings>;

CClass *ModSettings::GetNativeType() { return Red::GetClass<ModSettings>(); }

Memory::IAllocator *ModSettings::GetAllocator() { return Memory::RTTIAllocator::Get(); }

} // namespace ModSettings

RTTI_DEFINE_CLASS(ModSettings::ModSettings, "ModSettings", {
  RTTI_METHOD(GetInstance);
  RTTI_METHOD(GetMods);
  RTTI_METHOD(GetVars);
  RTTI_METHOD(GetCategories);
  RTTI_METHOD(AcceptChanges);
  RTTI_METHOD(RejectChanges);
  RTTI_METHOD(RestoreDefaults);
  RTTI_METHOD(RegisterListenerToClass);
  RTTI_METHOD(UnregisterListenerToClass);
  RTTI_METHOD(RegisterListenerToModifications);
  RTTI_METHOD(UnregisterListenerToModifications);
  RTTI_PROPERTY(changeMade);
});