#include "Addresses.hpp"
#include "ScriptData.hpp"
#include "ModSettings.hpp"
#include <Registrar.hpp>

/// @hash 3993832650
void *__fastcall ReleaseScriptData(ModSettings::ScriptData *scriptData);

REGISTER_HOOK(void *__fastcall, ReleaseScriptData, ModSettings::ScriptData *scriptData) {
  ModSettings::ModSettings::ProcessScriptData(scriptData);
  auto og = ReleaseScriptData_Original(scriptData);
  return og;
}