#pragma once
#include "IRuntimeVariable.hpp"
#include "ScriptDefinitions/ScriptProperty.hpp"
#include "ModSettings.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>

namespace ModSettings {

template <typename T> struct RuntimeVariable : public IRuntimeVariable {
  RuntimeVariable(ScriptProperty *prop) : IRuntimeVariable(prop) {
    T value, defaultValue;
    prop->ReadDefaultValue(&defaultValue);
    UpdateDefault(&defaultValue);
    value = defaultValue;
    ModSettings::ReadValueFromFile(prop, &value);
    UpdateAll(&value);
  }

  RuntimeVariable(RED4ext::CName className, RED4ext::CName propertyName, RED4ext::CName displayName,
                  RED4ext::CName description, uint32_t order, T defaultValue)
      : IRuntimeVariable(className, propertyName, displayName, description, order) {
    UpdateDefault(&defaultValue);
    T value = defaultValue;
    ModSettings::ReadValueFromFile(className, propertyName, &value);
    UpdateAll(&value);
  }

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

  virtual void __fastcall GetValueToWrite(char *value) override;

  virtual inline RED4ext::ScriptInstance *__fastcall GetInputValue() override {
    return (RED4ext::ScriptInstance *)&valueInput;
  }

  virtual inline RED4ext::ScriptInstance *__fastcall GetValuePtr() override {
    return (RED4ext::ScriptInstance *)&valueValidated;
  }

  virtual inline void __fastcall UpdateDefault(void *value) override { defaultValue = *(T *)value; }

  T valueValidated = 0;
  T defaultValue = 0;
  T valueInput = 0;
  T valueWrittenToFile = 0;
};

template <typename T> struct RuntimeVariableRange : RuntimeVariable<T> {
  inline RuntimeVariableRange(ScriptProperty *prop) : RuntimeVariable<T>(prop) {
    this->type = RED4ext::user::EConfigVarType::Int;
    prop->ReadProperty("ModSettings.step", &this->stepValue, (T)1);
    prop->ReadProperty("ModSettings.min", &this->minValue, (T)0);
    prop->ReadProperty("ModSettings.max", &this->maxValue, (T)10);
  }
  
  inline RuntimeVariableRange(CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             T defaultValue, T stepValue, T minValue, T maxValue)
      : RuntimeVariable<T>(className, propertyName, displayName, description, order, defaultValue) {
    this->type = RED4ext::user::EConfigVarType::Int;
    this->stepValue = stepValue;
    this->minValue = minValue;
    this->maxValue = maxValue;
  }

  T minValue;
  T maxValue;
  T stepValue;
};

template <typename T> struct RuntimeVariableList : public RuntimeVariable<uint32_t> {
  inline RuntimeVariableList(ScriptProperty *prop) : RuntimeVariable<uint32_t>(prop) {}
  inline RuntimeVariableList(CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             uint32_t defaultValue)
      : RuntimeVariable<uint32_t>(className, propertyName, displayName, description, order, defaultValue) {
  }
  T value;
  RED4ext::DynArray<T> values;
  RED4ext::DynArray<RED4ext::CName> displayValues;
};

struct RuntimeVariableBool : RuntimeVariable<bool> {
  inline RuntimeVariableBool(ScriptProperty *prop) : RuntimeVariable<bool>(prop) {
    this->type = RED4ext::user::EConfigVarType::Bool;
  }
  inline RuntimeVariableBool(CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             bool defaultValue)
      : RuntimeVariable<bool>(className, propertyName, displayName, description, order, defaultValue) {
    this->type = RED4ext::user::EConfigVarType::Bool;
  }
};

// i don't think these will matter here
// RED4EXT_ASSERT_SIZE(RuntimeVariableBool, 0x4E);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, valueValidated, 0x48);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, defaultValue, 0x49);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, valueInput, 0x4A);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, valueWrittenToFile, 0x4B);
// char (*__kaboom)[offsetof(RuntimeVariableBool, valueValidated)] = 1;

template <> inline RuntimeVariableRange<float>::RuntimeVariableRange(ScriptProperty *prop) : RuntimeVariable(prop) {
  this->type = RED4ext::user::EConfigVarType::Float;
  prop->ReadProperty("ModSettings.step", &this->stepValue, 0.05f);
  prop->ReadProperty("ModSettings.min", &this->minValue, 0.0f);
  prop->ReadProperty("ModSettings.max", &this->maxValue, 1.0f);
}

template <> inline void __fastcall RuntimeVariable<bool>::GetValueToWrite(char *value) {
  sprintf(value, "%d", valueValidated);
}

template <> inline void __fastcall RuntimeVariable<int32_t>::GetValueToWrite(char *value) {
  sprintf(value, "%d", valueValidated);
}

template <> inline void __fastcall RuntimeVariable<uint32_t>::GetValueToWrite(char *value) {
  sprintf(value, "%d", valueValidated);
}

template <> inline void __fastcall RuntimeVariable<float>::GetValueToWrite(char *value) {
  sprintf(value, "%f", valueValidated);
}

struct RuntimeVariableEnum : public RuntimeVariableList<int32_t> {
  RuntimeVariableEnum(ScriptProperty *prop) : RuntimeVariableList<int32_t>(prop) {
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
};

} // namespace ModSettings