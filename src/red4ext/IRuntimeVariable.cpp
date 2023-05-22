#include "IRuntimeVariable.hpp"
#include "ScriptDefinitions/ScriptProperty.hpp"

namespace ModSettings {

IRuntimeVariable::IRuntimeVariable() {
  bitfield.isInPreGame = true;
  bitfield.isInGame = true;
  bitfield.isVisible = true;
  bitfield.isInitialized = true;
  bitfield.isDisabled = false;
  bitfield.canBeRestoredToDefault = true;
}

IRuntimeVariable::IRuntimeVariable(ScriptProperty* prop) : IRuntimeVariable() {
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

IRuntimeVariable::IRuntimeVariable(RED4ext::CName className, RED4ext::CName propertyName, RED4ext::CName displayName, RED4ext::CName description, uint32_t order) : IRuntimeVariable() {
  this->name = propertyName;
  this->displayName = displayName;
  char str[0x100];
  std::sprintf(str, "/mods/%s/%s", className.ToString(), propertyName.ToString());
  this->groupPath = RED4ext::CNamePool::Add(str);
  this->description = description;
  this->order = order;
}

}