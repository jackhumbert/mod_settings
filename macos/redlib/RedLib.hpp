#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <format>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <RED4ext/GameEngine.hpp>
#include <RED4ext/RTTISystem.hpp>
#include <RED4ext/Scripting/Natives/ScriptGameInstance.hpp>
#include <RED4ext/Scripting/Natives/Generated/game/IGameSystem.hpp>
#include <RED4ext/Scripting/Natives/Generated/game/ScriptableSystem.hpp>

#include <nameof/nameof.hpp>

#include "Red/Alias.hpp"
#include "Red/Framework.hpp"
#include "Red/LogChannel.hpp"
#include "Red/TypeInfo.hpp"
#include "Red/Specializations.hpp"
#include "Red/Utils.hpp"
