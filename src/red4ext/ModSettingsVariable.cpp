#include "ModSettingsVariable.hpp"
#include "ModRuntimeSettingsVar.hpp"

void ModSettingsVariable::UpdateValues() {
  auto classType = RED4ext::CRTTISystem::Get()->GetClass(className);
  auto valuePtr = settingsVar->GetValuePtr();
  for (auto i = 0; i < classType->propertiesWithDefaults.size; i++) {
    if (classType->propertiesWithDefaults[i] == settingsVar->name) {
      uint64_t addr = classType->defaultValues[i]->type;
      auto propType = reinterpret_cast<RED4ext::CBaseRTTIType *>(addr & 0xFFFFFFFFFFFFFFF8);
      propType->Assign(&classType->defaultValues[i]->value, valuePtr);
    }
  }
  std::shared_lock<RED4ext::SharedMutex> _(listeners_lock);
  for (auto &listener : listeners) {
    if (listener && !listener->ref.Expired()) {
      auto prop = classType->propertiesByName.Get(settingsVar->name);
      if (prop && *prop) {
        (*prop)->SetValue(listener, valuePtr);
      }
    }
  }
}