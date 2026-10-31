#include <Hooks/ApplyOverrides.hpp>
#include <Registrar.hpp>
#include <RED4ext/HashMap.hpp>
#include <ModSettings.hpp>

// game::input::UserMapping::RawMappingCollection * game::input::UserMapping::GetMappingCollection(CName)
UserMapping::RawMappingCollection * UserMapping::GetMappingCollection(CName mapName) {
  static UniRelocFunc<decltype(&UserMapping::GetMappingCollection)> call(3702071356);
  return call(this, mapName);
}

// void game::input::UserMapping::ClearRuntimeDataOnRawMappings_Expensive() const
void UserMapping::ClearRuntimeDataOnRawMappings_Expensive() {
  static UniRelocFunc<decltype(&UserMapping::ClearRuntimeDataOnRawMappings_Expensive)> call(4196866905);
  return call(this);
}

// bool game::input::Manager::CheckOverrideConflicts(CName, unsigned short, DynArray<game::input::OverridableMapping> &) const
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
  this->contextManager->userMapping->ClearRuntimeDataOnRawMappings_Expensive();
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

decltype(&Manager_ApplyOverrides) Manager_ApplyOverrides_Original;
ModModuleHookHash s_Manager_ApplyOverrides_Hook("Manager_ApplyOverrides", 1773342692, reinterpret_cast<void*>(&Manager_ApplyOverrides), reinterpret_cast<void**>(&Manager_ApplyOverrides_Original));

// void game::input::Manager::ApplyOverrides()
void Manager_ApplyOverrides(Manager * manager) {
  ModSettings::ModSettings::AddOverrides(manager);
  Manager_ApplyOverrides_Original(manager);
}

// bool game::input::ContextManager::Override(CName, unsigned int, unsigned short)
// REGISTER_HOOK_HASH(bool, 1120015283, ContextManager_Override, ContextManager* cm, CName name, unsigned int ui, unsigned short key) {
//   return ContextManager_Override_Original(cm, name, ui, key);
// }

// bool ink::SkipKey(EInputKey)
REGISTER_HOOK_HASH(bool, 3298233196, SkipKey, uint32_t key) {
  return SkipKey_Original(key) && !((key - (uint32_t)EInputKey::IK_Pad_First) <= 0x13);
}

// #include <RED4ext/Scripting/Natives/Generated/ink/SettingsSelectorControllerKeyBinding.hpp>

// void ink::SettingsSelectorControllerKeyBinding::ListenForInput()
// REGISTER_HOOK_HASH(void, 1256790244, SettingsSelectorControllerKeyBinding_ListenForInput, ink::SettingsSelectorControllerKeyBinding * self) {
//   SettingsSelectorControllerKeyBinding_ListenForInput_Original(self);
//   return;
// }

// void ink::SettingsSelectorControllerKeyBinding::UnregisterRawInputLambda()
// REGISTER_HOOK_HASH(void, 1693129953, SettingsSelectorControllerKeyBinding_UnregisterRawInputLambda, ink::SettingsSelectorControllerKeyBinding * self) {
//   SettingsSelectorControllerKeyBinding_UnregisterRawInputLambda_Original(self);
//   return;
// }

// not named yet in IDA_Cyberpunk_2077
// REGISTER_HOOK_HASH(uint64_t, 3729004759, ListenForInputCallback, 
//   WeakHandle<ink::SettingsSelectorControllerKeyBinding> self, 
//   EInputKey key, // EInputKey
//   uint64_t type, // type
//   float duration
// ) {
//   auto og = ListenForInputCallback_Original(self, key, type, duration);
//   return og;
// }