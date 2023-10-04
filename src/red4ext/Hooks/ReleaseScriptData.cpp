#include "Addresses.hpp"
#include "ScriptData.hpp"
#include "ModSettings.hpp"
#include <Registrar.hpp>

/// @pattern 40 53 48 83 EC 20 48 8B D9 E8 92 00 00 00 48 8D 8B 38 01 00 00 FF 15 71 D5 2B 02 48 8D 8B 00 01
void *__fastcall ReleaseScriptData(ModSettings::ScriptData *scriptData);

REGISTER_HOOK(void *__fastcall, ReleaseScriptData, ModSettings::ScriptData *scriptData) {
  ModSettings::ModSettings::ProcessScriptData(scriptData);
  auto og = ReleaseScriptData_Original(scriptData);
  return og;
}