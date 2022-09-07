#pragma once
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/Common.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include "ModSettings.hpp"

struct ModRuntimeSettingsVar : public RED4ext::user::RuntimeSettingsVar {
  ModRuntimeSettingsVar();

  ModRuntimeSettingsVar(ScriptProperty* prop);

  virtual void __fastcall UpdateValue(void *value) override;

  // custom
  virtual void LoadValues(ScriptProperty *prop);
  virtual void __fastcall GetValueToWrite(char *value) = 0;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() = 0;
  virtual void __fastcall UpdateDefault(void *value) = 0;

  //RED4ext::CName typeName = RED4ext::CName();
};

struct ModRuntimeSettingsVarBool : public ModRuntimeSettingsVar {
  ModRuntimeSettingsVarBool(ScriptProperty *prop) : ModRuntimeSettingsVar(prop) { 
    type = RED4ext::user::EConfigVarType::Bool; 
    LoadValues(prop);
  }

  virtual bool __fastcall WasModifiedSinceLastSave() override;
  virtual bool __fastcall HasChange() override;
  virtual bool __fastcall IsDefault() override;
  virtual bool __fastcall RestoreDefault(char a1) override;
  virtual void __fastcall UpdateValue(void *value) override;
  virtual void __fastcall ApplyChange() override;
  virtual void __fastcall RevertChange() override;
  virtual void __fastcall ChangeWasWritten() override;
  virtual void __fastcall UpdateAll(void *value) override;

  virtual void __fastcall GetValueToWrite(char *value) override;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() override;
  virtual void __fastcall UpdateDefault(void *value) override;

  bool valueValidated = 0;
  bool defaultValue = 0;
  bool valueInput = 0;
  bool valueWrittenToFile = 0;
  uint32_t unk4C = 0;
};

struct ModRuntimeSettingsVarInt32 : public ModRuntimeSettingsVar {
  ModRuntimeSettingsVarInt32(ScriptProperty *prop);

  virtual bool __fastcall WasModifiedSinceLastSave() override;
  virtual bool __fastcall HasChange() override;
  virtual bool __fastcall IsDefault() override;
  virtual bool __fastcall RestoreDefault(char a1) override;
  virtual void __fastcall UpdateValue(void *value) override;
  virtual void __fastcall ApplyChange() override;
  virtual void __fastcall RevertChange() override;
  virtual void __fastcall ChangeWasWritten() override;
  virtual void __fastcall UpdateAll(void *value) override;

  virtual void __fastcall GetValueToWrite(char *value) override;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() override;
  virtual void __fastcall UpdateDefault(void *value) override;

  int32_t valueValidated = 0;
  int32_t defaultValue = 0;
  int32_t valueInput = 0;
  int32_t valueWrittenToFile = 0;
  int32_t minValue = 0;
  int32_t maxValue = 10;
  uint32_t stepValue = 1;
  uint32_t unk64 = 0;
};

struct ModRuntimeSettingsVarFloat : public ModRuntimeSettingsVar {
  ModRuntimeSettingsVarFloat(ScriptProperty *prop);

  virtual bool __fastcall WasModifiedSinceLastSave() override;
  virtual bool __fastcall HasChange() override;
  virtual bool __fastcall IsDefault() override;
  virtual bool __fastcall RestoreDefault(char a1) override;
  virtual void __fastcall UpdateValue(void *value) override;
  virtual void __fastcall ApplyChange() override;
  virtual void __fastcall RevertChange() override;
  virtual void __fastcall ChangeWasWritten() override;
  virtual void __fastcall UpdateAll(void *value) override;

  virtual void LoadValues(ScriptProperty *prop) override;
  virtual void __fastcall GetValueToWrite(char *value) override;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() override;
  virtual void __fastcall UpdateDefault(void *value) override;

  float valueValidated = 0;
  float defaultValue = 0;
  float valueInput = 0;
  float valueWrittenToFile = 0;
  float minValue = 0;
  float maxValue = 1.0f;
  float stepValue = 0.05f;
  uint32_t unk64 = 0;
};

struct ModRuntimeSettingsVarEnum : public ModRuntimeSettingsVar {
  ModRuntimeSettingsVarEnum(ScriptProperty *prop) : ModRuntimeSettingsVar(prop) {
    type = RED4ext::user::EConfigVarType::IntList;
    LoadValues(prop);

    values = RED4ext::DynArray<int32_t>(new RED4ext::Memory::DefaultAllocator());
    displayValues = RED4ext::DynArray<RED4ext::CName>(new RED4ext::Memory::DefaultAllocator());

    auto e = RED4ext::CRTTISystem::Get()->GetEnumByScriptName(prop->type->name);
    if (e) {
      for (const auto &value : e->hashList) {
          displayValues.EmplaceBack(value);
      }
      for (const auto &value : e->valueList) {
          values.EmplaceBack((int32_t)value);
      }

      bitfield.listHasDisplayValues = true;
    }
  }

  virtual bool __fastcall WasModifiedSinceLastSave() override;
  virtual bool __fastcall HasChange() override;
  virtual bool __fastcall IsDefault() override;
  virtual bool __fastcall RestoreDefault(char a1) override;
  virtual void __fastcall UpdateValue(void *value) override;
  virtual void __fastcall ApplyChange() override;
  virtual void __fastcall RevertChange() override;
  virtual void __fastcall ChangeWasWritten() override;
  virtual void __fastcall UpdateAll(void *value) override;

  virtual void __fastcall GetValueToWrite(char *value) override;
  virtual RED4ext::ScriptInstance *__fastcall GetValuePtr() override;
  virtual void __fastcall UpdateDefault(void *value) override;

  uint32_t unk48 = 0;
  uint32_t valueValidated = 0;
  uint32_t defaultValue = 0;
  uint32_t valueInput = 0;
  uint32_t valueWrittenToFile = 0;
  uint32_t unk5C = 0;
  RED4ext::DynArray<int32_t> values;
  RED4ext::DynArray<RED4ext::CName> displayValues;
};