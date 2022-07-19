#include "ModRuntimeSettingsVar.hpp"

ModRuntimeSettingsVar::ModRuntimeSettingsVar() : RED4ext::user::RuntimeSettingsVar() {
  bitfield.isInPreGame = true;
  bitfield.isInGame = true;
  bitfield.isVisible = true;
  bitfield.isInitialized = true;
  bitfield.isDisabled = false;
  bitfield.canBeRestoredToDefault = true;
}

ModRuntimeSettingsVar::ModRuntimeSettingsVar(ScriptProperty *prop) : ModRuntimeSettingsVar() {
  name = prop->name;
  auto className = prop->parent->name;
  groupPath = RED4ext::CNamePool::Add("/mods/" + *className.ToString() + *"/" + *name.ToString());

  auto displayStr = prop->runtimeProperties.Get("ModSettings.displayName");
  if (displayStr) {
    displayName = RED4ext::CNamePool::Add(displayStr->c_str());
  } else {
    displayName = name;
  }

  auto descriptionStr = prop->runtimeProperties.Get("ModSettings.description");
  if (descriptionStr) {
    description = RED4ext::CNamePool::Add(descriptionStr->c_str());
  }

  auto orderStr = prop->runtimeProperties.Get("ModSettings.order");
  if (orderStr) {
    RED4ext::CRTTISystem::Get()->GetType("Uint32")->FromString(&order, *orderStr);
  }
}

void ModRuntimeSettingsVar::LoadValues(ScriptProperty *prop) {
  auto typeName = prop->type->name;
  auto propType = RED4ext::CRTTISystem::Get()->GetType(typeName);
  uint64_t defaultValue = 0;
  void *defaultValue_p = &defaultValue;

  if (prop->defaultValues.size) {
    propType->FromString(defaultValue_p, prop->defaultValues[0]);
  }
  UpdateDefault(defaultValue_p);

  uint64_t value = defaultValue;
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

void __fastcall ModRuntimeSettingsVar::UpdateValue(void *value) { 
  auto ms = ModSettings::GetInstance();
  ms->changeMade = true;
}

// Bool

bool __fastcall ModRuntimeSettingsVarBool::WasModifiedSinceLastSave() {
  return importPolicy != RED4ext::user::EConfigVarImportPolicy::Ignore && valueWrittenToFile != valueValidated;
}

bool __fastcall ModRuntimeSettingsVarBool::HasChange() { return valueInput != valueValidated; }

bool __fastcall ModRuntimeSettingsVarBool::IsDefault() {
  uint8_t value;
  if (HasChange()) {
    value = valueInput;
  } else {
    value = valueValidated;
  }
  return value == defaultValue;
}

bool __fastcall ModRuntimeSettingsVarBool::RestoreDefault(char a1) {
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

void __fastcall ModRuntimeSettingsVarBool::UpdateValue(void *value) {   
  valueInput = *(uint8_t *)value;
  ModRuntimeSettingsVar::UpdateValue(value);
}

void __fastcall ModRuntimeSettingsVarBool::ApplyChange() { valueValidated = valueInput; }

void __fastcall ModRuntimeSettingsVarBool::RevertChange() { valueInput = valueValidated; }

void __fastcall ModRuntimeSettingsVarBool::ChangeWasWritten() { valueWrittenToFile = valueValidated; }

void __fastcall ModRuntimeSettingsVarBool::UpdateAll(void *value) {
  valueWrittenToFile = *(uint8_t *)value;
  valueInput = *(uint8_t *)value;
  valueValidated = *(uint8_t *)value;
}

void __fastcall ModRuntimeSettingsVarBool::GetValueToWrite(char *value) { sprintf(value, "%d", valueValidated); }

RED4ext::ScriptInstance *__fastcall ModRuntimeSettingsVarBool::GetValuePtr() { return (RED4ext::ScriptInstance *)&valueValidated; }

void __fastcall ModRuntimeSettingsVarBool::UpdateDefault(void *value) { defaultValue = *(uint8_t *)value; }


// Float

//bool __fastcall ModRuntimeSettingsVarFloat::WasModifiedSinceLastSave() {
//  return importPolicy != RED4ext::user::EConfigVarImportPolicy::Ignore && valueWrittenToFile != valueValidated;
//}
//
//bool __fastcall ModRuntimeSettingsVarFloat::HasChange() { return valueInput != valueValidated; }
//
//bool __fastcall ModRuntimeSettingsVarFloat::IsDefault() {
//  float value;
//  if (HasChange()) {
//    value = valueInput;
//  } else {
//    value = valueValidated;
//  }
//  return value == defaultValue;
//}
//
//bool __fastcall ModRuntimeSettingsVarFloat::RestoreDefault(char a1) {
//  auto wasDefault = IsDefault();
//  if (wasDefault)
//    return !wasDefault;
//  unk44 = a1;
//  if (((a1 - 2) & 0xFD) != 0) {
//    if (((a1 - 1) & 0xFD) != 0) {
//      if (!a1) {
//        UpdateValue(&defaultValue);
//        // UserSettings = GetUserSettings();
//        // AddSettingsDataToSettings(UserSettings, a1);
//      }
//      return !wasDefault;
//    }
//  UpdateImmediately:
//    UpdateValue(&defaultValue);
//    // v9 = GetUserSettings();
//    // sub_7FF62769C390(v9, a1);
//    return !wasDefault;
//  }
//  switch (updatePolicy) {
//  case RED4ext::user::EConfigVarUpdatePolicy::Disabled:
//    return !wasDefault;
//  case RED4ext::user::EConfigVarUpdatePolicy::Immediately:
//    goto UpdateImmediately;
//  case RED4ext::user::EConfigVarUpdatePolicy::ConfirmationRequired:
//    UpdateValue(&defaultValue);
//    // v8 = GetUserSettings();
//    // SettingsConfirmChange_0(v8, a1);
//    break;
//  case RED4ext::user::EConfigVarUpdatePolicy::RestartRequired:
//    UpdateValue(&defaultValue);
//    // v7 = GetUserSettings();
//    // SettingsRestartRequired_0(v7, a1);
//    break;
//  case RED4ext::user::EConfigVarUpdatePolicy::LoadLastCheckpointRequired:
//    UpdateValue(&defaultValue);
//    // v6 = GetUserSettings();
//    // SettingsLoadLastCheckpoint_0(v6, a1);
//    break;
//  default:
//    // LogError_f("E:\\R6.Release\\dev\\src\\common\\redConfig\\include\\inGameConfigVar.hpp", 44, line,
//    //            "Unknown in-game config var update policy (%d)", (unsigned __int8)a1->updatePolicy);
//    //__debugbreak();
//    break;
//  }
//  return !wasDefault;
//}
//
//void __fastcall ModRuntimeSettingsVarFloat::UpdateValue(void *value) { valueInput = *(float *)value; }
//
//void __fastcall ModRuntimeSettingsVarFloat::ApplyChange() { valueValidated = valueInput; }
//
//void __fastcall ModRuntimeSettingsVarFloat::RevertChange() { valueInput = valueValidated; }
//
//void __fastcall ModRuntimeSettingsVarFloat::ChangeWasWritten() { valueWrittenToFile = valueValidated; }
//
//void __fastcall ModRuntimeSettingsVarFloat::UpdateAll(void *value) {
//  valueWrittenToFile = *(float *)value;
//  valueInput = *(float *)value;
//  valueValidated = *(float *)value;
//}
//
//void __fastcall ModRuntimeSettingsVarFloat::GetValueToWrite(char *value) {
//  sprintf(value, "%f", valueValidated);
//}
//
//RED4ext::ScriptInstance *__fastcall ModRuntimeSettingsVarFloat::GetValuePtr() {
//  return (RED4ext::ScriptInstance *)&valueValidated;
//}
//
//void __fastcall ModRuntimeSettingsVarFloat::UpdateDefault(void *value) { defaultValue = *(float *)value; }

// Int

ModRuntimeSettingsVarInt32::ModRuntimeSettingsVarInt32(ScriptProperty *prop) : ModRuntimeSettingsVar(prop) {
  type = RED4ext::user::EConfigVarType::Int;
  auto propType = RED4ext::CRTTISystem::Get()->GetType(prop->type->name);

  auto stepStr = prop->runtimeProperties.Get("ModSettings.step");
  if (stepStr) {
    propType->FromString(&stepValue, *stepStr);
  }

  auto minStr = prop->runtimeProperties.Get("ModSettings.min");
  if (minStr) {
    propType->FromString(&minValue, *minStr);
  }

  auto maxStr = prop->runtimeProperties.Get("ModSettings.max");
  if (maxStr) {
    propType->FromString(&maxValue, *maxStr);
  }
  LoadValues(prop);
}

bool __fastcall ModRuntimeSettingsVarInt32::WasModifiedSinceLastSave() {
  return importPolicy != RED4ext::user::EConfigVarImportPolicy::Ignore && valueWrittenToFile != valueValidated;
}

bool __fastcall ModRuntimeSettingsVarInt32::HasChange() { return valueInput != valueValidated; }

bool __fastcall ModRuntimeSettingsVarInt32::IsDefault() {
  int32_t value;
  if (HasChange()) {
    value = valueInput;
  } else {
    value = valueValidated;
  }
  return value == defaultValue;
}

bool __fastcall ModRuntimeSettingsVarInt32::RestoreDefault(char a1) {
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

void __fastcall ModRuntimeSettingsVarInt32::UpdateValue(void *value) { 
  valueInput = *(int32_t *)value; 
  ModRuntimeSettingsVar::UpdateValue(value);
}

void __fastcall ModRuntimeSettingsVarInt32::ApplyChange() { valueValidated = valueInput; }

void __fastcall ModRuntimeSettingsVarInt32::RevertChange() { valueInput = valueValidated; }

void __fastcall ModRuntimeSettingsVarInt32::ChangeWasWritten() { valueWrittenToFile = valueValidated; }

void __fastcall ModRuntimeSettingsVarInt32::UpdateAll(void *value) {
  valueWrittenToFile = *(int32_t *)value;
  valueInput = *(int32_t *)value;
  valueValidated = *(int32_t *)value;
}

void __fastcall ModRuntimeSettingsVarInt32::GetValueToWrite(char *value) { sprintf(value, "%d", valueValidated); }

RED4ext::ScriptInstance *__fastcall ModRuntimeSettingsVarInt32::GetValuePtr() {
  return (RED4ext::ScriptInstance *)&valueValidated;
}

void __fastcall ModRuntimeSettingsVarInt32::UpdateDefault(void *value) { defaultValue = *(int32_t *)value; }

// IntList 

bool __fastcall ModRuntimeSettingsVarEnum::WasModifiedSinceLastSave() {
  return importPolicy != RED4ext::user::EConfigVarImportPolicy::Ignore && valueWrittenToFile != valueValidated;
}

bool __fastcall ModRuntimeSettingsVarEnum::HasChange() { return valueInput != valueValidated; }

bool __fastcall ModRuntimeSettingsVarEnum::IsDefault() {
  uint32_t value;
  if (HasChange()) {
    value = valueInput;
  } else {
    value = valueValidated;
  }
  return value == defaultValue;
}

bool __fastcall ModRuntimeSettingsVarEnum::RestoreDefault(char a1) {
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

void __fastcall ModRuntimeSettingsVarEnum::UpdateValue(void *value) { 
  valueInput = *(uint32_t *)value;
  ModRuntimeSettingsVar::UpdateValue(value);
}

void __fastcall ModRuntimeSettingsVarEnum::ApplyChange() { valueValidated = valueInput; }

void __fastcall ModRuntimeSettingsVarEnum::RevertChange() { valueInput = valueValidated; }

void __fastcall ModRuntimeSettingsVarEnum::ChangeWasWritten() { valueWrittenToFile = valueValidated; }

void __fastcall ModRuntimeSettingsVarEnum::UpdateAll(void *value) {
  valueWrittenToFile = *(uint32_t *)value;
  valueInput = *(uint32_t *)value;
  valueValidated = *(uint32_t *)value;
}

void __fastcall ModRuntimeSettingsVarEnum::GetValueToWrite(char *value) {
  sprintf(value, "%d", valueValidated);
}

RED4ext::ScriptInstance *__fastcall ModRuntimeSettingsVarEnum::GetValuePtr() {
  return (RED4ext::ScriptInstance *)&valueValidated;
}

void __fastcall ModRuntimeSettingsVarEnum::UpdateDefault(void *value) { defaultValue = *(uint32_t *)value; }
