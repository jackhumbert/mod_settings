#include <Hooks/ApplyOverrides.hpp>
#include <Registrar.hpp>
#include <RED4ext/HashMap.hpp>
#include <ModSettings.hpp>

UserMapping::RawMappingCollection * UserMapping::GetMappingCollection(CName mapName) {
  static UniRelocFunc<decltype(&UserMapping::GetMappingCollection)> call(3702071356);
  return call(this, mapName);
}

bool Manager::Override(
    // CName contextName, 
    Overridable overridableUI, 
    uint16_t inputKey
) {
  bool status = false;
  for (auto & action : this->contextManager->actions.values) {
    for (auto & mapName : action->data.mapNames) {
      auto mappingCollection = this->contextManager->userMapping->GetMappingCollection(mapName);
      for (auto & mapping : mappingCollection->mappings) {
        if (mapping.overridableUI == overridableUI) {
          if (mapping.key16 != inputKey) {
            mapping.key16 = inputKey;
          }
          status = true;
        }
      }
    }
  }
  return status;
}

decltype(&ApplyOverrides) ApplyOverrides_Original;
ModModuleHookHash s_ApplyOverrides_Hook("ApplyOverrides", 1773342692, reinterpret_cast<void*>(&ApplyOverrides), reinterpret_cast<void**>(&ApplyOverrides_Original)); 

void ApplyOverrides(Manager * manager) {
  ModSettings::ModSettings::AddOverrides(manager);
  ApplyOverrides_Original(manager);
}
