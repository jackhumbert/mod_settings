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
  // static UniRelocFunc<decltype(&Manager::Override)> call(2948927698);
  // call(this, contextName, overridableUI, inputKey);
  bool status = false;
  for (auto & action : this->contextManager->actions.values) {
    for (auto & mapName : action->data.mapNames) {
      auto mappingCollection = this->contextManager->userMapping->GetMappingCollection(mapName);
      for (auto & mapping : mappingCollection->mappings) {
        if (mapping.overridableUI == overridableUI && mapping.key16 != inputKey) {
          mapping.key16 = inputKey;
          status = true;
        }
      }
    }
  }
  return status;
}

// enum EMappingType : uint8_t {
//   Internal = 0, // maybe
//   Button = 1,
//   FakeButton = 2,
//   Axis = 3,
//   FakeAxis = 4,
//   Relative = 5,
//   Unk6 = 6 
// };

// #pragma pack(push, 1)
// struct Mapping {
//   EMappingType type;
//   // 210 / 0xD2 - pre apply 175 25
//   //  35 / 0x23 - post apply 35 35
//   // 116 / 0x74 - pre apply 2 252 45
//   //  35 / 0x23 - post apply 2 35 35
//   //  35 / 0x23 - pre settings apply
//   //  35 / 0x23 - post settings apply
//   uint8_t unk01; // set to 0x23 after apply
//   EInputKey key16;
//   EInputKey key32;
//   uint16_t unk08;
//   CName name;
//   CName presetName;
//   float val;
//   float thresholdPress;
//   float thresholdRelease;
//   uint32_t overridableUI; // Java's String.hashCode()
//   uint32_t unk28;
//   uint32_t unk2C;
// };
// #pragma pack(pop)

// struct MappingCollection {
//   DynArray<Mapping> mappings;
// };

// struct RawMappingCollection {
//   DynArray<Mapping> mappings;
//   // set to 0 after an ApplyOverrides
//   bool has_update;
// };

// struct UserMapping {
//   DynArray<void *> unk00; // 00
//   uint8_t sorted; // 10
//   DynArray<void *>  unk18; // 18
//   CString invalid_str; // 28
//   void * contextManager; // 48
//   void * buttons; // 50
//   void * buttonGroups; // 58
//   void * axes; // 60
//   void * relativeAxes; // 68
//   // updated from actionMap on save
//   Map<EInputKey, MappingCollection> keyMap; // 70
//   Map<CName, RawMappingCollection> actionMap; // 98
//   HashMap<CString, EInputKey> stringToKey; // C0
//   HashMap<CString, EInputKey> inputKeyMap_F0; // F0
//   HashMap<EInputKey, CString> unk120; // 120
// };

// after OnStreamingWorldLoaded
// unk00 & unk18 empty
// baseMap 144
// overrides 462
// inputKeyMap_C0 272
// inputKeyMap_F0 272

// .xml binding
// ApplyOverrides()
// UserSettings.json applied

// after OnWorldAttached
// updated: 
// contextManager
// buttons
// buttonGroups
// axes
// relativeAxes

// .xml binding
// ApplyOverrides()
// UserSettings.json applied

// after "Apply" is hit

// class ContextManager {

// };

//  called once after Override
REGISTER_HOOK_HASH(void, 1773342692, ApplyOverrides, Manager * manager) {
  // manager->Override("VehicleDrive", "vehicleFlight", EInputKey::IK_D);
  // manager->Override("BaseVehicleFlight", "vehicleFlight", EInputKey::IK_F);
  ModSettings::ModSettings::AddOverrides(manager);
  ApplyOverrides_Original(manager);
}

//  called once after Override
// REGISTER_HOOK_HASH(void, 3016757648, ApplyOverrides, UserMapping *userMapping) {
//   auto hash = CalcHash("vehicleFlight");
//   userMapping->actionMap.ForEach([hash](const CName & action, RawMappingCollection & mappingCollection) {
//     bool updated = false;
//     for (auto mapping : mappingCollection.mappings) {
//       if (mapping.overridableUI == hash) {
//         updated = true;
//         mapping.key16 = EInputKey::IK_L;
//         mapping.key32 = EInputKey::IK_L;
//       }
//     }
//     mappingCollection.has_update = true;
//   });
//   ApplyOverrides_Original(userMapping);
// }

// called per key when updating
// REGISTER_HOOK_HASH(void, 1120015283, ContextManager_Override, 
//     ContextManager * contextManager, 
//     CName contextName,
//     unsigned int overridableUI, 
//     EInputKey inputKey) {
//   ContextManager_Override_Original(contextManager, contextName, overridableUI, inputKey);
// }
