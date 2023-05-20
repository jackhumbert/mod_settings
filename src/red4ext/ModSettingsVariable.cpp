#include <ModSettingsVariable.hpp>
#include "ModRuntimeSettingsVar.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"

void ModSettingsVariable::UpdateValues() {
  auto classType = RED4ext::CRTTISystem::Get()->GetClass(className);
  if (classType) {
    auto valuePtr = settingsVar->GetValuePtr();
    for (auto i = 0; i < classType->defaults.keys.size; i++) {
      if (classType->defaults.keys[i] == settingsVar->name) {
        sdk->logger->InfoF(pluginHandle, "Loaded %s.%s", className.ToString() , settingsVar->name.ToString());
        auto propType = classType->defaults.values[i]->GetType();
        classType->defaults.values[i]->Fill(propType, valuePtr);
      }
    }
    std::shared_lock<RED4ext::SharedMutex> _(listeners_lock);
    for (auto &listener : listeners) {
      if (listener && !listener->ref.Expired()) {
        auto prop = classType->propsByName.Get(settingsVar->name);
        if (prop && *prop) {
          (*prop)->SetValue(listener, valuePtr);
        }
      }
    }
  }
}