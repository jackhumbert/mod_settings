#pragma once
// macOS replacement for deps/cpcmake/include/CNames.hpp. The original declares the game's name registry through a Windows address
// hash (s_CNames, 1913530389); Mod Settings never reads it. Nothing is declared here so no unmapped hash is embedded.
#include <RED4ext/RED4ext.hpp>
#include <RED4ext/CName.hpp>
