#include "Addresses.hpp"
#include "ScriptData.hpp"
#include "ModSettings.hpp"
#include <Registrar.hpp>

/// @pattern 48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B D9 E8 99 0B 00 00 48 8D 8B 38 01 00 00 FF 15
void *__fastcall ReleaseScriptData(ModSettings::ScriptData *scriptData);

REGISTER_HOOK(void *__fastcall, ReleaseScriptData, ModSettings::ScriptData *scriptData) {
  ModSettings::ModSettings::ProcessScriptData(scriptData);
  auto og = ReleaseScriptData_Original(scriptData);
  return og;
}