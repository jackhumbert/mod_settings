#pragma once
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "Scripting/RTTIClass.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVar.hpp>
#include "ModRuntimeSettingsVar.hpp"

class ModConfigVarBool : public Engine::RTTIClass<ModConfigVarBool, RED4ext::user::SettingsVar> {
public:
  bool GetValue() { 
    return reinterpret_cast<ModRuntimeSettingsVarBool *>(this->runtimeVar)->valueInput;
  }

  void SetValue(bool value) { 
    reinterpret_cast<ModRuntimeSettingsVarBool *>(this->runtimeVar)->UpdateValue(&value);
  }

  bool GetDefaultValue() { 
    return reinterpret_cast<ModRuntimeSettingsVarBool *>(this->runtimeVar)->defaultValue;
  }

private:
  friend Descriptor;

  static void OnDescribe(Descriptor *aType, RED4ext::CRTTISystem *) {
    aType->AddFunction<&ModConfigVarBool::GetValue>("GetValue");
    aType->AddFunction<&ModConfigVarBool::SetValue>("SetValue");
    aType->AddFunction<&ModConfigVarBool::GetDefaultValue>("GetDefaultValue");
  }
};

class ModConfigVarFloat : public Engine::RTTIClass<ModConfigVarFloat, RED4ext::user::SettingsVar> {
public:
  float GetValue() { return reinterpret_cast<ModRuntimeSettingsVarFloat *>(this->runtimeVar)->valueInput; }

  void SetValue(float value) {
    reinterpret_cast<ModRuntimeSettingsVarFloat *>(this->runtimeVar)->UpdateValue(&value);
  }

  float GetDefaultValue() {
    return reinterpret_cast<ModRuntimeSettingsVarFloat *>(this->runtimeVar)->defaultValue;
  }

  float GetMinValue() { return reinterpret_cast<ModRuntimeSettingsVarFloat *>(this->runtimeVar)->minValue; }

  float GetMaxValue() { return reinterpret_cast<ModRuntimeSettingsVarFloat *>(this->runtimeVar)->maxValue; }

  float GetStepValue() { return reinterpret_cast<ModRuntimeSettingsVarFloat *>(this->runtimeVar)->stepValue; }

private:
  friend Descriptor;

  static void OnDescribe(Descriptor *aType, RED4ext::CRTTISystem *) {
    aType->AddFunction<&ModConfigVarFloat::GetValue>("GetValue");
    aType->AddFunction<&ModConfigVarFloat::SetValue>("SetValue");
    aType->AddFunction<&ModConfigVarFloat::GetDefaultValue>("GetDefaultValue");
    aType->AddFunction<&ModConfigVarFloat::GetMinValue>("GetMinValue");
    aType->AddFunction<&ModConfigVarFloat::GetMaxValue>("GetMaxValue");
    aType->AddFunction<&ModConfigVarFloat::GetStepValue>("GetStepValue");
  }
};

class ModConfigVarInt32 : public Engine::RTTIClass<ModConfigVarInt32, RED4ext::user::SettingsVar> {
public:
  int32_t GetValue() { return reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->valueInput; }

  void SetValue(int32_t value) {
    reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->UpdateValue(&value);
  }

  int32_t GetDefaultValue() {
    return reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->defaultValue;
  }

  int32_t GetMinValue() { return reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->minValue; }

  int32_t GetMaxValue() { return reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->maxValue; }

  int32_t GetStepValue() { return reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->stepValue; }

private:
  friend Descriptor;

  static void OnDescribe(Descriptor *aType, RED4ext::CRTTISystem *) {
    aType->AddFunction<&ModConfigVarInt32::GetValue>("GetValue");
    aType->AddFunction<&ModConfigVarInt32::SetValue>("SetValue");
    aType->AddFunction<&ModConfigVarInt32::GetDefaultValue>("GetDefaultValue");
    aType->AddFunction<&ModConfigVarInt32::GetMinValue>("GetMinValue");
    aType->AddFunction<&ModConfigVarInt32::GetMaxValue>("GetMaxValue");
    aType->AddFunction<&ModConfigVarInt32::GetStepValue>("GetStepValue");
  }
};

class ModConfigVarEnum : public Engine::RTTIClass<ModConfigVarEnum, RED4ext::user::SettingsVar> {
public:
  int32_t GetValueFor(int32_t index) {
    auto varEnum = reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar);
    if (varEnum->values.size > index) {
      return varEnum->values[index];
    } else {
      return 0;
    }
  }

  int32_t GetValue() {
    return GetValueFor(reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar)->valueInput);
  }

  int32_t GetDefaultValue() {
    return GetValueFor(reinterpret_cast<ModRuntimeSettingsVarInt32 *>(this->runtimeVar)->defaultValue);
  }

  RED4ext::DynArray<int32_t> GetValues() {
    auto varEnum = reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar);
    auto values = RED4ext::DynArray<int32_t>(new RED4ext::Memory::DefaultAllocator());
    for (const auto &value : varEnum->values) {
      values.EmplaceBack(value);
    }
    return values;
  }

  int32_t GetIndexFor(int32_t value) {
    auto varEnum = reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar);
    auto values = RED4ext::DynArray<int32_t>(new RED4ext::Memory::DefaultAllocator());
    int32_t index = -1;
    for (int i = 0; i < varEnum->values.size; i++) {
      if (varEnum->values[i] == value) {
        index = i;
      }
    }
    return index;
  }

  int32_t GetIndex() { return reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar)->valueInput; }

  int32_t GetDefaultIndex() {
    return reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar)->defaultValue;
  }

  void SetIndex(int32_t index) {
    reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar)->UpdateValue(&index);
  }

  RED4ext::CName GetDisplayValue(int32_t index) {
    auto varEnum = reinterpret_cast<ModRuntimeSettingsVarEnum *>(this->runtimeVar);
    if (varEnum->displayValues.size > index && index > -1) {
      return varEnum->displayValues[index];
    } else {
      return "None";
    }
  }

private:
  friend Descriptor;

  static void OnDescribe(Descriptor *aType, RED4ext::CRTTISystem *) {
    aType->AddFunction<&ModConfigVarEnum::GetValueFor>("GetValueFor");
    aType->AddFunction<&ModConfigVarEnum::GetValue>("GetValue");
    aType->AddFunction<&ModConfigVarEnum::GetDefaultValue>("GetDefaultValue");
    aType->AddFunction<&ModConfigVarEnum::GetValues>("GetValues");
    aType->AddFunction<&ModConfigVarEnum::GetIndexFor>("GetIndexFor");
    aType->AddFunction<&ModConfigVarEnum::GetIndex>("GetIndex");
    aType->AddFunction<&ModConfigVarEnum::GetDefaultIndex>("GetDefaultIndex");
    aType->AddFunction<&ModConfigVarEnum::SetIndex>("SetIndex");
    aType->AddFunction<&ModConfigVarEnum::GetDisplayValue>("GetDisplayValue");
  }
};