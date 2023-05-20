#pragma once
#include "IModRuntimeSettingsVar.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include <ModSettings.hpp>
#include <RED4ext/Common.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

template <typename T> struct ModRuntimeSettingsVar : IModRuntimeSettingsVar {
  inline ModRuntimeSettingsVar(ScriptProperty *prop) : IModRuntimeSettingsVar(prop) { this->LoadValues(prop); }

  // overrides

  virtual inline bool __fastcall WasModifiedSinceLastSave() override {
    return importPolicy != RED4ext::user::EConfigVarImportPolicy::Ignore && valueWrittenToFile != valueValidated;
  }

  virtual inline bool __fastcall HasChange() override { return valueInput != valueValidated; }

  virtual inline bool __fastcall IsDefault() override {
    T value;
    if (HasChange()) {
      value = valueInput;
    } else {
      value = valueValidated;
    }
    return value == defaultValue;
  }

  virtual inline bool __fastcall RestoreDefault(uint8_t a1) override {
    auto wasDefault = IsDefault();
    if (wasDefault)
      return !wasDefault;
    unk44 = a1;
    if (((a1 - 2) & 0xFD) != 0) {
      if (((a1 - 1) & 0xFD) != 0) {
        if (!a1) {
          UpdateValue(&defaultValue);
          // UserSettings = GetUserSettings();
          // AddSettingsDataToSettings(UserSettings, a1);
        }
        return !wasDefault;
      }
    UpdateImmediately:
      UpdateValue(&defaultValue);
      // v9 = GetUserSettings();
      // sub_7FF62769C390(v9, a1);
      return !wasDefault;
    }
    switch (updatePolicy) {
    case RED4ext::user::EConfigVarUpdatePolicy::Disabled:
      return !wasDefault;
    case RED4ext::user::EConfigVarUpdatePolicy::Immediately:
      goto UpdateImmediately;
    case RED4ext::user::EConfigVarUpdatePolicy::ConfirmationRequired:
      UpdateValue(&defaultValue);
      // v8 = GetUserSettings();
      // SettingsConfirmChange_0(v8, a1);
      break;
    case RED4ext::user::EConfigVarUpdatePolicy::RestartRequired:
      UpdateValue(&defaultValue);
      // v7 = GetUserSettings();
      // SettingsRestartRequired_0(v7, a1);
      break;
    case RED4ext::user::EConfigVarUpdatePolicy::LoadLastCheckpointRequired:
      UpdateValue(&defaultValue);
      // v6 = GetUserSettings();
      // SettingsLoadLastCheckpoint_0(v6, a1);
      break;
    default:
      // LogError_f("E:\\R6.Release\\dev\\src\\common\\redConfig\\include\\inGameConfigVar.hpp", 44, line,
      //            "Unknown in-game config var update policy (%d)", (unsigned __int8)a1->updatePolicy);
      //__debugbreak();
      break;
    }
    return !wasDefault;
  }

  virtual inline void __fastcall UpdateValue(void *value) override {
    valueInput = *(T *)value;
    auto ms = ModSettings::GetInstance();
    ms->changeMade = true;
    ms->NotifyListeners();
  }

  virtual inline void __fastcall ApplyChange() override { valueValidated = valueInput; }

  virtual inline void __fastcall RevertChange() override { valueInput = valueValidated; }

  virtual inline void __fastcall ChangeWasWritten() override { valueWrittenToFile = valueValidated; }

  virtual inline void __fastcall UpdateAll(void *value) override {
    valueWrittenToFile = *(T *)value;
    valueInput = *(T *)value;
    valueValidated = *(T *)value;
  }

  // custom virtuals

  virtual inline void LoadValues(ScriptProperty *prop) override {
    auto typeName = prop->type->name;
    auto propType = RED4ext::CRTTISystem::Get()->GetType(typeName);
    T defaultValue;
    void *defaultValue_p = &defaultValue;

    if (prop->defaultValues.size) {
      propType->FromString(defaultValue_p, prop->defaultValues[0]);
    }
    UpdateDefault(defaultValue_p);

    T value = defaultValue;
    void *value_p = &value;

    RED4ext::CString settingFromFile;
    if (ModSettings::GetSettingString(prop->parent->name, name, &settingFromFile)) {
      propType->FromString(value_p, settingFromFile);
    }
    UpdateAll(value_p);

    if (prop->defaultValues.size) {
      propType->ToString(value_p, prop->defaultValues[0]);
    }
  }

  virtual void __fastcall GetValueToWrite(char *value) override;

  virtual inline RED4ext::ScriptInstance *__fastcall GetValuePtr() override {
    return (RED4ext::ScriptInstance *)&valueValidated;
  }

  virtual inline void __fastcall UpdateDefault(void *value) override { defaultValue = *(T *)value; }

  T valueValidated = 0;
  T defaultValue = 0;
  T valueInput = 0;
  T valueWrittenToFile = 0;
};

template <typename T> struct ModRuntimeSettingsVarRange : ModRuntimeSettingsVar<T> {
  inline ModRuntimeSettingsVarRange(ScriptProperty *prop) : ModRuntimeSettingsVar<T>(prop) {
    this->type = RED4ext::user::EConfigVarType::Int;
    prop->ReadProperty("ModSettings.step", &this->stepValue);
    prop->ReadProperty("ModSettings.min", &this->minValue);
    prop->ReadProperty("ModSettings.max", &this->maxValue);
  }

  T minValue;
  T maxValue;
  T stepValue;
};

template <typename T> struct ModRuntimeSettingsVarList : ModRuntimeSettingsVar<uint32_t> {
  ModRuntimeSettingsVarList(ScriptProperty *prop) : ModRuntimeSettingsVar<uint32_t>(prop) {}

  T value;
  RED4ext::DynArray<T> values;
  RED4ext::DynArray<RED4ext::CName> displayValues;
};

/**
 * @brief A toggle switch
 */
// template <>
// struct ModRuntimeSettingsVar<bool> : IModRuntimeSettingsVar {
//   ModRuntimeSettingsVar(ScriptProperty *prop) : IModRuntimeSettingsVar(prop) {
//     this->type = RED4ext::user::EConfigVarType::Bool;
//   }

//   virtual void __fastcall GetValueToWrite(char *value) override { sprintf(value, "%d", valueValidated); }

//   // uint32_t unk4C = 0;
// };

template <>
inline ModRuntimeSettingsVar<bool>::ModRuntimeSettingsVar(ScriptProperty *prop) : IModRuntimeSettingsVar(prop) {
  this->type = RED4ext::user::EConfigVarType::Bool;
}

template <> inline void __fastcall ModRuntimeSettingsVar<bool>::GetValueToWrite(char *value) {
  sprintf(value, "%d", valueValidated);
}

// RED4EXT_ASSERT_SIZE(ModRuntimeSettingsVarBool, 0x4E);
RED4EXT_ASSERT_OFFSET(ModRuntimeSettingsVar<bool>, valueValidated, 0x48);
RED4EXT_ASSERT_OFFSET(ModRuntimeSettingsVar<bool>, defaultValue, 0x49);
RED4EXT_ASSERT_OFFSET(ModRuntimeSettingsVar<bool>, valueInput, 0x4A);
RED4EXT_ASSERT_OFFSET(ModRuntimeSettingsVar<bool>, valueWrittenToFile, 0x4B);
// char (*__kaboom)[offsetof(ModRuntimeSettingsVarBool, valueValidated)] = 1;

/**
 * @brief A range setting of int32_t
 */
struct ModRuntimeSettingsVarInt32 : public ModRuntimeSettingsVarRange<int32_t> {
  ModRuntimeSettingsVarInt32(ScriptProperty *prop) : ModRuntimeSettingsVarRange(prop) {
    this->type = RED4ext::user::EConfigVarType::IntList;
    this->minValue = 0;
    this->maxValue = 10;
    this->stepValue = 1;
  }

  virtual inline void __fastcall GetValueToWrite(char *value) override { sprintf(value, "%d", valueValidated); }

  uint32_t unk64 = 0;
};

/**
 * @brief A range setting of floats
 */
struct ModRuntimeSettingsVarFloat : public ModRuntimeSettingsVarRange<float> {
  ModRuntimeSettingsVarFloat(ScriptProperty *prop) : ModRuntimeSettingsVarRange(prop) {
    this->type = RED4ext::user::EConfigVarType::Float;
    this->minValue = 0;
    this->maxValue = 1.0;
    this->stepValue = 0.05;
  }

  virtual inline void __fastcall GetValueToWrite(char *value) override { sprintf(value, "%f", valueValidated); }

  uint32_t unk64 = 0;
};

struct ModRuntimeSettingsVarEnum : public ModRuntimeSettingsVarList<int32_t> {
  ModRuntimeSettingsVarEnum(ScriptProperty *prop) : ModRuntimeSettingsVarList<int32_t>(prop) {
    this->type = RED4ext::user::EConfigVarType::IntList;

    values = RED4ext::DynArray<int32_t>(new RED4ext::Memory::DefaultAllocator());
    displayValues = RED4ext::DynArray<RED4ext::CName>(new RED4ext::Memory::DefaultAllocator());

    auto e = RED4ext::CRTTISystem::Get()->GetEnumByScriptName(prop->type->name);
    if (e) {
      const auto displayValuesPrefix = RED4ext::FNV1a64("ModSettings.displayValues.");

      for (const auto &value : e->hashList) {
        const auto displayValueAttr = RED4ext::FNV1a64(value.ToString(), displayValuesPrefix);
        const auto displayValue = prop->runtimeProperties.Get(displayValueAttr);
        if (displayValue) {
          RED4ext::CNamePool::Add(displayValue->c_str());
          displayValues.EmplaceBack(displayValue->c_str());
        } else {
          displayValues.EmplaceBack(value);
        }
      }

      for (const auto &value : e->valueList) {
        values.EmplaceBack((int32_t)value);
      }

      bitfield.listHasDisplayValues = true;
    }
  }

  virtual inline void __fastcall GetValueToWrite(char *value) override { sprintf(value, "%d", valueValidated); }
};