#pragma once
// macOS only. Is this the game's script bundle? (R41c-2, item d)
//
// The ScriptBundle destructor (0x21ECFD8) runs for every bundle the game destroys. Mod Settings' ProcessScriptData clears its mod list and
// rebuilds it from the bundle it is handed, so a destroyed bundle that is not the game's (empty, partly built, a test bundle) would wipe
// the list. Codeware decides with Mac::ScriptsValidated(): a flag set after its ScriptValidator::Validate hook returned for the bundle
// (macos/MacEngine.cpp, OnMacScriptBundleDestroyed). That flag is hidden inside Codeware.dylib and Codeware is not edited here, so this
// plugin uses what Codeware's own layout guard (MacScriptValidator.cpp CheckLayout) accepts as the game's bundle:
//   * classes (+0xD0) is a sane DynArray with at least one class, and the first classes have a vtable inside the game image, a printable
//     name and a plausible parent / properties header;
//   * definitionsByName (+0x00) finds ISerializable and IScriptable, the two roots of every game class hierarchy.
// Every read goes through vm_read_overwrite first. A false answer is logged once per reason and the original destructor still runs.
#include <string>

namespace ModSettingsMac
{
// True when aBundle looks like the game's validated script bundle. aWhy gets the first failed check otherwise.
bool IsGameScriptBundle(const void* aBundle, std::string& aWhy, unsigned* aClassCount = nullptr);
} // namespace ModSettingsMac
