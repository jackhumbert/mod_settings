#pragma once
#include "RED4ext/Scripting/Natives/userSettingsVar.hpp"
#include "RuntimeVariable.hpp"
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "Scripting/RTTIClass.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVar.hpp>
#include <RedLib.hpp>

// namespace ModSettings {

template <typename T> struct ModConfigVar : RED4ext::IScriptable {
public:
  T GetValue() { return this->runtimeVar->valueInput; }
  void SetValue(T value) { this->runtimeVar->UpdateValue(&value); }
  T GetDefaultValue() { return this->runtimeVar->defaultValue; }

  ModSettings::RuntimeVariable<T> *runtimeVar; // 40

  RTTI_IMPL_TYPEINFO(ModConfigVar);
  RTTI_IMPL_ALLOCATOR();
};

template <typename T> struct ModConfigVarRange : RED4ext::IScriptable {
  T GetValue() { return this->runtimeVar->valueInput; }
  void SetValue(T value) { this->runtimeVar->UpdateValue(&value); }
  T GetDefaultValue() { return this->runtimeVar->defaultValue; }
  T GetMinValue() { return this->runtimeVar->minValue; }
  T GetMaxValue() { return this->runtimeVar->maxValue; }
  T GetStepValue() { return this->runtimeVar->stepValue; }
  
  ModSettings::RuntimeVariableRange<T> *runtimeVar; // 40

  RTTI_IMPL_TYPEINFO(ModConfigVarRange);
  RTTI_IMPL_ALLOCATOR();
};

RTTI_DEFINE_CLASS(ModConfigVar<bool>, "ModConfigVarBool", {
  RTTI_PARENT(RED4ext::user::SettingsVar);
  RTTI_METHOD(GetValue);
  RTTI_METHOD(SetValue);
  RTTI_METHOD(GetDefaultValue);
});

RTTI_DEFINE_CLASS(ModConfigVarRange<float>, "ModConfigVarFloat", {
  RTTI_PARENT(RED4ext::user::SettingsVar);
  RTTI_METHOD(GetValue);
  RTTI_METHOD(SetValue);
  RTTI_METHOD(GetDefaultValue);
  RTTI_METHOD(GetMinValue);
  RTTI_METHOD(GetMaxValue);
  RTTI_METHOD(GetStepValue);
});

RTTI_DEFINE_CLASS(ModConfigVarRange<int32_t>, "ModConfigVarInt32", {
  RTTI_PARENT(RED4ext::user::SettingsVar);
  RTTI_METHOD(GetValue);
  RTTI_METHOD(SetValue);
  RTTI_METHOD(GetDefaultValue);
  RTTI_METHOD(GetMinValue);
  RTTI_METHOD(GetMaxValue);
  RTTI_METHOD(GetStepValue);
});

struct ModConfigVarEnum : RED4ext::IScriptable {
  int32_t GetValueFor(int32_t index) {
    auto varEnum = reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar);
    if (varEnum->values.size > index) {
      return varEnum->values[index];
    } else {
      return 0;
    }
  }

  int32_t GetValue() { return GetValueFor(reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar)->valueInput); }

  int32_t GetDefaultValue() {
    return GetValueFor(reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar)->defaultValue);
  }

  RED4ext::DynArray<int32_t> GetValues() {
    auto varEnum = reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar);
    auto values = RED4ext::DynArray<int32_t>(new RED4ext::Memory::DefaultAllocator());
    for (const auto &value : varEnum->values) {
      values.EmplaceBack(value);
    }
    return values;
  }

  int32_t GetIndexFor(int32_t value) {
    auto varEnum = reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar);
    auto values = RED4ext::DynArray<int32_t>(new RED4ext::Memory::DefaultAllocator());
    int32_t index = -1;
    for (int i = 0; i < varEnum->values.size; i++) {
      if (varEnum->values[i] == value) {
        index = i;
      }
    }
    return index;
  }

  int32_t GetIndex() { return reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar)->valueInput; }

  int32_t GetDefaultIndex() { return reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar)->defaultValue; }

  void SetIndex(int32_t index) { reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar)->UpdateValue(&index); }

  RED4ext::CName GetDisplayValue(int32_t index) {
    auto varEnum = reinterpret_cast<ModSettings::RuntimeVariableEnum *>(this->runtimeVar);
    if (varEnum->displayValues.size > index && index > -1) {
      return varEnum->displayValues[index];
    } else {
      return "None";
    }
  }

  ModSettings::RuntimeVariableEnum *runtimeVar; // 40

  RTTI_IMPL_TYPEINFO(ModConfigVarEnum);
  RTTI_IMPL_ALLOCATOR();
};

RTTI_DEFINE_CLASS(ModConfigVarEnum, {
  RTTI_PARENT(RED4ext::user::SettingsVar);
  RTTI_METHOD(GetValue);
  RTTI_METHOD(GetValueFor);
  RTTI_METHOD(GetDefaultValue);
  RTTI_METHOD(GetValues);
  RTTI_METHOD(GetIndexFor);
  RTTI_METHOD(GetDefaultIndex);
  RTTI_METHOD(SetIndex);
  RTTI_METHOD(GetDisplayValue);
});