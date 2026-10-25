// #include "Addresses.hpp"
#include "ScriptData.hpp"
#include "ModSettings.hpp"
#include <Registrar.hpp>

// @hash 3993832650
// void *__fastcall ReleaseScriptData(ModSettings::CScriptedDataEnvironment *scriptData);
// RED4ext::UniversalRelocFunc<void (*)(ModSettings::CScriptedDataEnvironment *)> ReleaseScriptData(3993832650);

REGISTER_HOOK_HASH(void *, 3993832650, ReleaseScriptData, ModSettings::CScriptedDataEnvironment *scriptData) {
  ModSettings::ModSettings::ProcessScriptData(scriptData);
  auto og = ReleaseScriptData_Original(scriptData);
  return og;
}