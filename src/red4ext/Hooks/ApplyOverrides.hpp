#pragma once

#include <RED4ext/RED4ext.hpp>
#include <RED4ext/CName.hpp>
#include <RED4ext/CString.hpp>
#include <RED4ext/Scripting/Natives/Generated/EInputKey.hpp>

using namespace RED4ext;

const static UniversalRelocFunc<uint32_t (*)(const CString &)> CalcHash(2583235069);

struct Overridable {
  Overridable(const char* str) : m_hash(CalcHash(str)) { }
  
  inline bool operator==(const Overridable& rhs) const {
    return this->m_hash == rhs.m_hash;
  }

  uint32_t m_hash;
};

struct ContextManager;

struct UserMappingAxis {
  struct RealAxisInput {
    uint16_t key;
    uint16_t unk02;
    uint32_t unk04;
  };

  struct Unknown {
    uint32_t unk00;
    uint32_t unk04;
    uint32_t unk08;
  };
  RED4EXT_ASSERT_SIZE(Unknown, 0xC);

  struct Axis {  
    DynArray<RealAxisInput> realAxisInput;
    DynArray<Unknown> unk10;
    uint8_t invert;
    uint8_t unk21;
    uint64_t unk28;
  };
  RED4EXT_ASSERT_SIZE(Axis, 0x30);

  struct Pair {
    CName name1;
    CName name2;
    CName name3;
    void * axis;
    uint64_t unk20;
    CName name4;
    float unk30;
    uint32_t unk34;
  };

  HashMap<CName, Axis> axes;
  HashMap<CName, float> floats;
  DynArray<Pair *> pairs;
};

struct UserMapping {
  struct Mapping {
    uint8_t type;
    uint8_t unk01;
    uint16_t key16;
    EInputKey key32;
    CName name;
    CName presetName;
    float val;
    float thresholdPress;
    float thresholdRelease;
    Overridable overridableUI;
    uint32_t unk28;
    uint32_t unk2C;
  };
  RED4EXT_ASSERT_SIZE(Mapping, 0x30);

  struct MappingCollection {
    DynArray<Mapping> actions;
  };

  struct RawMappingCollection {
    DynArray<Mapping> mappings;
    uint8_t needs_update;
  };

  struct MappingPreset {
    CName name;
    uint8_t axis;
    uint8_t fakeAxis;
    uint8_t relativeAxis;
  };

  RawMappingCollection * GetMappingCollection(CName);
  void ClearRuntimeDataOnRawMappings();
  
  DynArray<void*> unk00; // 00
  uint8_t sorted; // 10
  DynArray<void*>  unk18; // 18
  CString invalid_str; // 28
  ContextManager * contextManager; // 48
  void * buttons; // 50
  void * buttonGroups; // 58
  UserMappingAxis * axes; // 60
  void * relativeAxes; // 68
  Map<uint16_t, MappingCollection> keyMap; // 70
  Map<CName, RawMappingCollection> rawMappings; // 98
  HashMap<CString, EInputKey> stringToKey; // C0
  HashMap<CString, EInputKey> inputKeyMap_F0; // F0
  HashMap<EInputKey, CString> unk120; // 120
  uint64_t unk150[4];
  DynArray<MappingPreset*> mappingPresets;
  uint8_t unk180;
  uint8_t unk181;
  uint64_t unk188;
  uint64_t listener_id;

};

struct ContextManager {

  struct ActionData {
    CName actionName;
    DynArray<CName> mapNames;
    void *unk18;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    uint32_t unk38;
    uint32_t flags;
    uint64_t unk40[5];
    DynArray<CName> unk68_cname;
    uint64_t unk98[5];
    DynArray<CName> unkA0_cname;
    uint64_t unkB0[5];
    DynArray<CName> unkD8_cname;
    uint8_t unkE8;
    uint64_t unkF0;
  };

  struct Action {
    ActionData data;
    DynArray<void*> messageListeners;
    DynArray<void*> unk108;
    DynArray<void*> unk118;
    DynArray<void*> unk128;
  };

  void *__vftable;
  UserMapping * userMapping;
  DynArray<void*> deferredListenerActions;
  uint64_t deferredListenerActionsMutex;
  Map<CName, Action*> actions;
  uint32_t unk4C;
  Map<CName, void*> mappingGroups;
  uint32_t unk74;
  Map<CName, void*> actionGroups;
  uint32_t unk9C;
  Map<CName, void*> unkA0;
  uint64_t mappingGroup;
  uint64_t unkD0[6];
  DynArray<CName> contexts;
  DynArray<void*> unk110;
  DynArray<void*> unk120;
  DynArray<void*> unk130;
  uint8_t unk140[8];
  void *contextTransition;
  uint8_t unk148[152];
  CString unk14;
  CName LeftStick;
  CName RightStick;
  uint64_t unk218;
  uint64_t unk220;
  uint8_t inputDeviceRelatedNot12;
};

struct OverridableMapping {

};

struct Manager {
  enum OverrideStatus {
    NotFound = 0,
    NotReplaced = 1,
    Replaced = 2
  };

  OverrideStatus Override(
    // set in XML with <context name="[contextName]">
    // uses separate "Settings*" context names, specifically for key binding in settings
    // goes into userSettings group name as "/key_bindings/[contextName]"
    // CName contextName, 
    // set in XML as "overridableUI"
    // needs to exist in [contextName], but not tied to it
    // (cannot override keys differently in different contexts)
    Overridable overridableUI, 
    uint16_t inputKey,
    uint32_t callNumber);
    
  // bool CheckOverrideConflicts(
  //   CName, 
  //   unsigned short, 
  //   DynArray<OverridableMapping> &) const;


  void * __vftable;
  void *managerBackend;
  ContextManager *contextManager;
  uint32_t unk18;
  uint8_t unk1C;
  uint8_t overrideSet;
  uint64_t contextMutex;
  uint32_t unk28;
  int32_t lastOverrideStaus;
  DynArray<void*> validators;
  DynArray<void*> unk40;
  uint64_t listener_id;
};

void ApplyOverrides(Manager * manager);