#include <Hooks/ApplyOverrides.hpp>
#include <Registrar.hpp>
#include <RED4ext/HashMap.hpp>
#include <ModSettings.hpp>

UserMapping::RawMappingCollection * UserMapping::GetMappingCollection(CName mapName) {
  static UniRelocFunc<decltype(&UserMapping::GetMappingCollection)> call(3702071356);
  return call(this, mapName);
}

void UserMapping::ClearRuntimeDataOnRawMappings() {
  static UniRelocFunc<decltype(&UserMapping::ClearRuntimeDataOnRawMappings)> call(4196866905);
  return call(this);
}

// bool Manager::CheckOverrideConflicts(CName contextName, unsigned short key, DynArray<OverridableMapping> & conflicts) const {
//   static UniRelocFunc<decltype(&Manager::CheckOverrideConflicts)> call(1600530764);
//   return call(this, contextName, key, conflicts);
// }

Manager::OverrideStatus Manager::Override(
    // CName contextName, 
    Overridable overridableUI, 
    uint16_t inputKey,
    uint32_t callNumber
) {
  uint32_t status = NotFound;
  // CheckForOverrideConflicts
  this->contextManager->userMapping->ClearRuntimeDataOnRawMappings();
  for (const auto & action : this->contextManager->actions.values) {
    for (const auto & mapName : action->data.mapNames) {
      auto mappingCollection = this->contextManager->userMapping->GetMappingCollection(mapName);
      // CheckForOverrideConflicts
      if (!mappingCollection || mappingCollection->needs_update == 1)
        continue;
      mappingCollection->needs_update = 1;
      // Override
      for (auto & mapping : mappingCollection->mappings) {
        if (mapping.overridableUI == overridableUI) {
          if (mapping.key16 != inputKey) {
            mapping.key16 = inputKey;
            status |= Replaced;
            if (callNumber > 1) {
              // game::input::Manager::HandleVarChange
              if (this->lastOverrideStaus <= 1)
                this->lastOverrideStaus = 1;
              this->overrideSet = 1;
            }
          } else {
            status |= NotReplaced;
          }
        }
      }
    }
  }
  return (Manager::OverrideStatus)status;
}

decltype(&ApplyOverrides) ApplyOverrides_Original;
ModModuleHookHash s_ApplyOverrides_Hook("ApplyOverrides", 1773342692, reinterpret_cast<void*>(&ApplyOverrides), reinterpret_cast<void**>(&ApplyOverrides_Original)); 

void ApplyOverrides(Manager * manager) {
  ModSettings::ModSettings::AddOverrides(manager);
  ApplyOverrides_Original(manager);
}

// REGISTER_HOOK_HASH(bool, 1120015283, Override, ContextManager* cm, CName name, unsigned int ui, unsigned short key) {
//   return Override_Original(cm, name, ui, key);
// }

REGISTER_HOOK_HASH(bool, 3298233196, SkipKey, uint32_t key) {
  return SkipKey_Original(key) && !((key - (uint32_t)EInputKey::IK_Pad_First) <= 0x13);
}

// #include <RED4ext/Scripting/Natives/Generated/ink/SettingsSelectorControllerKeyBinding.hpp>

// REGISTER_HOOK_HASH(void, 1256790244, ListenForInput, ink::SettingsSelectorControllerKeyBinding * self) {
//   ListenForInput_Original(self);
//   return;
// }

// REGISTER_HOOK_HASH(void, 1693129953, UnregisterRawInputLambda, ink::SettingsSelectorControllerKeyBinding * self) {
//   UnregisterRawInputLambda_Original(self);
//   return;
// }

// REGISTER_HOOK_HASH(uint64_t, 3729004759, ListenForInputCallback, 
//   WeakHandle<ink::SettingsSelectorControllerKeyBinding> self, 
//   EInputKey key, // EInputKey
//   uint64_t type, // type
//   float duration
// ) {
//   auto og = ListenForInputCallback_Original(self, key, type, duration);
//   return og;
// }