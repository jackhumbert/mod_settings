// #include "Addresses.hpp"
#include "ScriptData.hpp"
#include "ModSettings.hpp"
#include <Registrar.hpp>
#ifdef __APPLE__
#include "MacBundle.hpp"
#endif

// @hash 3993832650
// void *__fastcall ReleaseScriptData(ModSettings::ScriptData *scriptData);
// RED4ext::UniversalRelocFunc<void (*)(ModSettings::ScriptData *)> ReleaseScriptData(3993832650);

#ifdef __APPLE__
// Mac: ScriptBundle_dtor (0x21ECFD8) is a void function, the Windows prototype returns whatever rax holds. The target is also hooked by
// Codeware (HookAfter<ScriptBundle::Destruct>), so this hook joins Codeware's chain: the detour runs first and calls the previous
// detour as its "original".
REGISTER_HOOK_HASH_SHARED(void, 3993832650, ReleaseScriptData, ModSettings::ScriptData *scriptData) {
  // Only the game's bundle is processed: ProcessScriptData clears the mod list and rebuilds it from the bundle it gets, so an empty or foreign
  // bundle destroyed after the real one would wipe the list. Codeware makes the same decision with its "validated" flag (macos/MacBundle.hpp).
  std::string why;
  unsigned classCount = 0;
  if (ModSettingsMac::IsGameScriptBundle(scriptData, why, &classCount)) {
    ModSettingsMac::Log("ReleaseScriptData: the game's script bundle (%u classes), reading the Mod Settings variables", classCount);
    ModSettings::ModSettings::ProcessScriptData(scriptData);
  } else {
    ModSettingsMac::LogOnce(("bundle-skip-" + why).c_str(), "ReleaseScriptData: a script bundle was skipped (not the game's: %s)", why.c_str());
  }
  ReleaseScriptData_Original(scriptData);
}
#else
REGISTER_HOOK_HASH(void *, 3993832650, ReleaseScriptData, ModSettings::ScriptData *scriptData) {
  ModSettings::ModSettings::ProcessScriptData(scriptData);
  auto og = ReleaseScriptData_Original(scriptData);
  return og;
}
#endif