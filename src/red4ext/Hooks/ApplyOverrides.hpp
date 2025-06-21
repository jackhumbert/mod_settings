#pragma once

#include <RED4ext/RED4ext.hpp>
#include <RED4ext/CName.hpp>
#include <RED4ext/CString.hpp>
#include <RED4ext/Scripting/Natives/Generated/EInputKey.hpp>

const static RED4ext::UniversalRelocFunc<uint32_t (*)(const RED4ext::CString &)> CalcHash(2583235069);

struct Overridable {
  Overridable(const char* str) : m_hash(CalcHash(str)) { }

  uint32_t m_hash;
};

struct Manager {
  void Override(
    // set in XML with <context name="[contextName]">
    // uses separate "Settings*" context names, specifically for key binding in settings
    // goes into userSettings group name as "/key_bindings/[contextName]"
    RED4ext::CName contextName, 
    // set in XML as "overridableUI"
    // needs to exist in [contextName], but not tied to it
    // (cannot override keys differently in different contexts)
    Overridable overridableUI, 
    uint16_t inputKey);
};