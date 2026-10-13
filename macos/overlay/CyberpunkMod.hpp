#pragma once
// macOS replacement for deps/cpcmake/include/CyberpunkMod.hpp. The original is Windows only (<winnt.h>, QueryPerformanceCounter for
// a profiler Mod Settings does not use). Only what Main.cpp needs from it: the hook and module registry.
#include <stdint.h>
#include <map>
#include "CNames.hpp"
#include "Registrar.hpp"
