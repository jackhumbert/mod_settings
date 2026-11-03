#pragma once
// macOS overlay for jackhumbert's userSettingsVar.hpp (the Mac SDK has the class in Generated/user/SettingsVar.hpp, as an
// opaque 0x48-byte IScriptable). Mod Settings' ModConfigVar types keep their own typed runtimeVar pointer at +0x40.
#include <RED4ext/Scripting/Natives/Generated/user/SettingsVar.hpp>
#include <RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp>
