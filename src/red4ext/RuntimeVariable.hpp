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
    T _value, _defaultValue;
    prop->ReadDefaultValue(&_defaultValue);
    SetDefaultValue(&_defaultValue);
    _value = _defaultValue;
    ModSettings::ReadValueFromFile(prop, &_value);
    LoadValue(&_value);
  }

  RuntimeVariable(RED4ext::CName modName, RED4ext::CName className, RED4ext::CName propertyName, RED4ext::CName displayName,
                  RED4ext::CName description, uint32_t order, T _defaultValue)
      : IRuntimeVariable(modName, className, propertyName, displayName, description, order) {
    SetDefaultValue(&_defaultValue);
    T _value = _defaultValue;
    ModSettings::ReadValueFromFile(className, propertyName, &_value);
    LoadValue(&_value);
  }

  // overrides

  virtual inline bool __fastcall WasModifiedSinceLastSave() override {
    return importPolicy != RED4ext::user::EConfigVarImportPolicy::Ignore && savedValue != acceptedValue;
  }

  virtual inline bool __fastcall HasChange() override { return requestedValue != acceptedValue; }

  virtual inline bool __fastcall IsDefault() override {
    T value;
    if (HasChange()) {
      value = requestedValue;
    } else {
      value = acceptedValue;
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
          SetRequestedValue(&defaultValue);
          // UserSettings = GetUserSettings();
          // AddSettingsDataToSettings(UserSettings, a1);
        }
        return !wasDefault;
      }
    UpdateImmediately:
      SetRequestedValue(&defaultValue);
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
      SetRequestedValue(&defaultValue);
      // v8 = GetUserSettings();
      // SettingsConfirmChange_0(v8, a1);
      break;
    case RED4ext::user::EConfigVarUpdatePolicy::RestartRequired:
      SetRequestedValue(&defaultValue);
      // v7 = GetUserSettings();
      // SettingsRestartRequired_0(v7, a1);
      break;
    case RED4ext::user::EConfigVarUpdatePolicy::LoadLastCheckpointRequired:
      SetRequestedValue(&defaultValue);
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

  virtual inline void __fastcall SetRequestedValue(void *value) override {
    requestedValue = *(T *)value;

    auto ms = ModSettings::GetInstance();
    ms->changesRequested |= requestedValue != acceptedValue;
    ms->NotifyListenersRequested(this->groupPath, this->name);
  }

  virtual inline void __fastcall AcceptChange() override { 
    acceptedValue = requestedValue;

    auto ms = ModSettings::GetInstance();
    ms->NotifyListenersAccepted(this->groupPath, this->name);
  }

  virtual inline void __fastcall RejectChange() override { 
    requestedValue = acceptedValue;
  }

  virtual inline void __fastcall MarkAsSaved() override { 
    savedValue = acceptedValue;
  }

  virtual inline void __fastcall LoadValue(void *value) override {
    savedValue = *(T *)value;
    requestedValue = *(T *)value;
    acceptedValue = *(T *)value;
  }

  // custom virtuals

  virtual void __fastcall GetValueToWrite(char *value) override;

  virtual inline RED4ext::ScriptInstance *__fastcall GetRequestedValue() override {
    return (RED4ext::ScriptInstance *)&requestedValue;
  }

  virtual inline RED4ext::ScriptInstance *__fastcall GetAcceptedValue() override {
    return (RED4ext::ScriptInstance *)&acceptedValue;
  }

  virtual inline void __fastcall SetDefaultValue(void *value) override { 
    defaultValue = *(T *)value;
  }

  T acceptedValue;
  T defaultValue;
  T requestedValue;
  T savedValue;
};

template <typename T> struct RuntimeVariableRange : RuntimeVariable<T> {
  inline RuntimeVariableRange(ScriptProperty *prop) : RuntimeVariable<T>(prop) {
    this->type = RED4ext::user::EConfigVarType::Int;
    prop->ReadProperty("ModSettings.step", &this->stepValue, (T)1);
    prop->ReadProperty("ModSettings.min", &this->minValue, (T)0);
    prop->ReadProperty("ModSettings.max", &this->maxValue, (T)10);
  }
  
  inline RuntimeVariableRange(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             T defaultValue, T stepValue, T minValue, T maxValue)
      : RuntimeVariable<T>(modName, className, propertyName, displayName, description, order, defaultValue) {
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
  inline RuntimeVariableList(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             uint32_t defaultValue)
      : RuntimeVariable<uint32_t>(modName, className, propertyName, displayName, description, order, defaultValue) {
  }
  T value;
  RED4ext::DynArray<T> values;
  RED4ext::DynArray<RED4ext::CName> displayValues;
};

struct RuntimeVariableBool : RuntimeVariable<bool> {
  inline RuntimeVariableBool(ScriptProperty *prop) : RuntimeVariable<bool>(prop) {
    this->type = RED4ext::user::EConfigVarType::Bool;
  }
  inline RuntimeVariableBool(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             bool defaultValue)
      : RuntimeVariable<bool>(modName, className, propertyName, displayName, description, order, defaultValue) {
    this->type = RED4ext::user::EConfigVarType::Bool;
  }
};

struct RuntimeVariableKeyBinding : RuntimeVariable<EInputKey> {
  inline RuntimeVariableKeyBinding(ScriptProperty *prop) : RuntimeVariable<EInputKey>(prop) {
    this->type = RED4ext::user::EConfigVarType::Name;
  }
  inline RuntimeVariableKeyBinding(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             EInputKey defaultValue)
      : RuntimeVariable<EInputKey>(modName, className, propertyName, displayName, description, order, defaultValue) {
    this->type = RED4ext::user::EConfigVarType::Name;
  }
};

struct RuntimeVariableName : RuntimeVariable<CName> {
  inline RuntimeVariableName(ScriptProperty *prop) : RuntimeVariable<CName>(prop) {
    this->type = RED4ext::user::EConfigVarType::Name;
  }
  inline RuntimeVariableName(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             CName defaultValue)
      : RuntimeVariable<CName>(modName, className, propertyName, displayName, description, order, defaultValue) {
    this->type = RED4ext::user::EConfigVarType::Name;
  }
};

// i don't think these will matter here
// RED4EXT_ASSERT_SIZE(RuntimeVariableBool, 0x4E);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, acceptedValue, 0x48);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, defaultValue, 0x49);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, requestedValue, 0x4A);
// RED4EXT_ASSERT_OFFSET(RuntimeVariable<bool>, savedValue, 0x4B);
// char (*__kaboom)[offsetof(RuntimeVariableBool, acceptedValue)] = 1;

template <> inline RuntimeVariableRange<float>::RuntimeVariableRange(ScriptProperty *prop) : RuntimeVariable(prop) {
  this->type = RED4ext::user::EConfigVarType::Float;
  prop->ReadProperty("ModSettings.step", &this->stepValue, 0.05f);
  prop->ReadProperty("ModSettings.min", &this->minValue, 0.0f);
  prop->ReadProperty("ModSettings.max", &this->maxValue, 1.0f);
}

template <> inline RuntimeVariableRange<float>::RuntimeVariableRange(CName modName, CName className, CName propertyName, CName displayName, CName description, uint32_t order,
                             float defaultValue, float stepValue, float minValue, float maxValue)
      : RuntimeVariable<float>(modName, className, propertyName, displayName, description, order, defaultValue) {
    this->type = RED4ext::user::EConfigVarType::Float;
    this->stepValue = stepValue;
    this->minValue = minValue;
    this->maxValue = maxValue;
  }

template <> inline void __fastcall RuntimeVariable<bool>::GetValueToWrite(char *value) {
  sprintf(value, "%d", acceptedValue);
}

template <> inline void __fastcall RuntimeVariable<int32_t>::GetValueToWrite(char *value) {
  sprintf(value, "%d", acceptedValue);
}

template <> inline void __fastcall RuntimeVariable<uint32_t>::GetValueToWrite(char *value) {
  sprintf(value, "%d", acceptedValue);
}

template <> inline void __fastcall RuntimeVariable<float>::GetValueToWrite(char *value) {
  sprintf(value, "%f", acceptedValue);
}

template <> inline void __fastcall RuntimeVariable<EInputKey>::GetValueToWrite(char *value) {
  sprintf(value, "%u", acceptedValue);
}

// not suppported in the UI yet
template <> inline void __fastcall RuntimeVariable<CName>::GetValueToWrite(char *value) {
  sprintf(value, "%s", acceptedValue.ToString());
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