// #include "Addresses.hpp"
#include "ScriptData.hpp"
#include "ModSettings.hpp"
#include <Registrar.hpp>

// CScriptedDataEnvironment::~CScriptedDataEnvironment()
REGISTER_HOOK_HASH(void *, 3993832650, CScriptedDataEnvironment_dtor, ModSettings::CScriptedDataEnvironment *scriptData) {
  ModSettings::ModSettings::ProcessScriptData(scriptData);
  auto og = CScriptedDataEnvironment_dtor_Original(scriptData);
  return og;
}
