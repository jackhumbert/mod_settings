#include "IModRuntimeSettingsVar.hpp"
#include "ScriptDefinitions/ScriptProperty.hpp"

IModRuntimeSettingsVar::IModRuntimeSettingsVar() {
  bitfield.isInPreGame = true;
  bitfield.isInGame = true;
  bitfield.isVisible = true;
  bitfield.isInitialized = true;
  bitfield.isDisabled = false;
  bitfield.canBeRestoredToDefault = true;
}

IModRuntimeSettingsVar::IModRuntimeSettingsVar(ScriptProperty* prop) : IModRuntimeSettingsVar() {
  this->updatePolicy = RED4ext::user::EConfigVarUpdatePolicy::ConfirmationRequired;
  this->name = prop->name;
  char str[0x100];
  std::sprintf(str, "/mods/%s/%s", prop->parent->name.ToString(), this->name.ToString());
  this->groupPath = RED4ext::CNamePool::Add(str);

  // prop->ReadProperty("ModSettings.updatePolicy", &updatePolicy, RED4ext::user::EConfigVarUpdatePolicy::ConfirmationRequired);
  prop->ReadProperty("ModSettings.displayName", &this->displayName, this->name);
  prop->ReadProperty("ModSettings.description", &this->description);
  prop->ReadProperty("ModSettings.order", &this->order);

}