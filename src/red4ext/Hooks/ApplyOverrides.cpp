#include <Hooks/ApplyOverrides.hpp>
#include <Registrar.hpp>
#include <RED4ext/HashMap.hpp>
#include <ModSettings.hpp>
#ifdef __APPLE__
#include "MacShims.hpp"
#endif

#ifdef __APPLE__
// R41b: Mac addresses (ModSettingsSdkRows.inc). this in x0, the CName as a 64-bit hash in x1.
UserMapping::RawMappingCollection * UserMapping::GetMappingCollection(CName mapName) {
  static UniversalRelocFunc<UserMapping::RawMappingCollection *(*)(UserMapping *, CName)> call(3702071356);
  return call(this, mapName);
}

void UserMapping::ClearRuntimeDataOnRawMappings() {
  static UniversalRelocFunc<void (*)(UserMapping *)> call(4196866905);
  return call(this);
}
#else
UserMapping::RawMappingCollection * UserMapping::GetMappingCollection(CName mapName) {
  static UniRelocFunc<decltype(&UserMapping::GetMappingCollection)> call(3702071356);
  return call(this, mapName);
}

void UserMapping::ClearRuntimeDataOnRawMappings() {
  static UniRelocFunc<decltype(&UserMapping::ClearRuntimeDataOnRawMappings)> call(4196866905);
  return call(this);
}
#endif

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

#ifdef __APPLE__
bool MacUserMappingUsable(UserMapping * userMapping) {
  // UserMapping::contextManager (+0x48) is the one field of the override path that R41b could not match against a Windows twin. Read it
  // without faulting and check the back pointer (ContextManager::userMapping, +8, verified).
  uint64_t contextManager = 0;
  uint64_t backPointer = 0;
  if (!userMapping || !ModSettingsMac::SafeRead(reinterpret_cast<const uint8_t *>(userMapping) + offsetof(UserMapping, contextManager), contextManager) ||
      contextManager == 0 || (contextManager & 7) != 0 ||
      !ModSettingsMac::SafeRead(reinterpret_cast<const uint8_t *>(contextManager) + offsetof(ContextManager, userMapping), backPointer) ||
      backPointer != reinterpret_cast<uint64_t>(userMapping)) {
    ModSettingsMac::LogOnce("usermapping-check",
                            "key-binding overrides are skipped: UserMapping::contextManager (+0x48) %p does not point back at the mapping %p "
                            "(R41b could not verify that field)",
                            reinterpret_cast<void *>(contextManager), static_cast<void *>(userMapping));
    return false;
  }
  return true;
}

Manager::OverrideStatus OverrideKeyMac(UserMapping * userMapping, Overridable overridableUI, uint16_t inputKey, uint32_t callNumber) {
  uint32_t status = Manager::NotFound;
  userMapping->ClearRuntimeDataOnRawMappings();
  for (const auto & action : userMapping->contextManager->actions.values) {
    for (const auto & mapName : action->data.mapNames) {
      auto mappingCollection = userMapping->GetMappingCollection(mapName);
      if (!mappingCollection || mappingCollection->needs_update == 1)
        continue;
      mappingCollection->needs_update = 1;
      for (auto & mapping : mappingCollection->mappings) {
        if (mapping.overridableUI == overridableUI) {
          if (mapping.key16 != inputKey) {
            mapping.key16 = inputKey;
            status |= Manager::Replaced;
          } else {
            status |= Manager::NotReplaced;
          }
        }
      }
    }
  }
  if (callNumber > 1) {
    // Manager::Override sets overrideSet = 1 and lastOverrideStaus = 1 here (the game's HandleVarChange notification). The body gets only the
    // UserMapping, so those two Manager fields cannot be set; writing them through the vtable chain would patch game memory.
    ModSettingsMac::LogOnce("override-status-fields",
                            "Manager::overrideSet and lastOverrideStaus are not set on Mac (the ApplyOverrides body gets only the UserMapping); "
                            "key bindings still apply, the game's change notification for them is not raised");
  }
  return (Manager::OverrideStatus)status;
}
#endif

decltype(&ApplyOverrides) ApplyOverrides_Original;
#ifndef __APPLE__
ModModuleHookHash s_ApplyOverrides_Hook("ApplyOverrides", 1773342692, reinterpret_cast<void*>(&ApplyOverrides), reinterpret_cast<void**>(&ApplyOverrides_Original)); 
#endif

void ApplyOverrides(Manager * manager) {
  ModSettings::ModSettings::AddOverrides(manager);
#ifdef __APPLE__
  // Not hooked on the Mac (the Windows hook sits on a thunk that only exists there): there is no original to call.
  if (!ApplyOverrides_Original)
    return;
#endif
  ApplyOverrides_Original(manager);
}

#ifdef __APPLE__
#ifdef MODSETTINGS_MAC_APPLYOVERRIDES
// R41b CONFIRMED: the target is the body 0x1D792E4, a real entry of void(UserMapping*). On by default; -DMODSETTINGS_MAC_APPLYOVERRIDES=OFF
// compiles the hook out (then 0x1d792e4 must also leave hook-offsets.txt).
decltype(&ApplyOverridesMac) ApplyOverridesMac_Original;
ModModuleHookHash s_ApplyOverridesMac_Hook("ApplyOverrides(body)", 1773342692, reinterpret_cast<void*>(&ApplyOverridesMac), reinterpret_cast<void**>(&ApplyOverridesMac_Original));

void ApplyOverridesMac(UserMapping * userMapping) {
  ModSettings::ModSettings::AddOverridesMac(userMapping);
  if (ApplyOverridesMac_Original)
    ApplyOverridesMac_Original(userMapping);
}
#else
void ApplyOverridesMac(UserMapping *) {}
#endif
#endif

// REGISTER_HOOK_HASH(bool, 1120015283, Override, ContextManager* cm, CName name, unsigned int ui, unsigned short key) {
//   return Override_Original(cm, name, ui, key);
// }

#ifndef __APPLE__
#ifndef __APPLE__
REGISTER_HOOK_HASH(bool, 3298233196, SkipKey, uint32_t key) {
  return SkipKey_Original(key) && !((key - (uint32_t)EInputKey::IK_Pad_First) <= 0x13);
}
#else
// Mac: no SkipKey hook. R41b: 0x48026F0 (R40's match for hash 3298233196) has no reference of any kind on Mac (no bl, b, adr or data
// reference); its only caller's twin, 0x48038B8, inlines the predicate. The hook could never fire, so it is not installed. Effect: the key
// binding capture does not drop gamepad-range keys (IK_Pad_First .. +0x13) from the keyboard binding on Mac, as in vanilla.
#endif
#else
// Mac: no SkipKey hook. R41b: 0x48026F0 (R40's match for hash 3298233196) has no reference of any kind on Mac (no bl, b, adr or data
// reference); its only caller's twin, 0x48038B8, inlines the predicate. The hook could never fire, so it is not installed. Effect: the key
// binding capture does not drop gamepad-range keys (IK_Pad_First .. +0x13) from the keyboard binding on Mac, as in vanilla.
#endif

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