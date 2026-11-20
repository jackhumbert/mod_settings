#pragma once

// A list variable's saved text as an index into its values (RuntimeVariableEnum's C++ constructor), kept apart from
// the engine types so tests/ can link it without the game.

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace ModSettings {

// aText's index among aNames (the enum's value names, in order): a value's name, a Bool saved under the same name
// (false or true: the first or second value, as ModVariable::Write writes a Bool), or a number in range; aFallback
// for anything else.
inline uint32_t EnumIndexFromText(const std::string &aText, const std::vector<std::string> &aNames, uint32_t aFallback) {
  for (uint32_t i = 0; i < aNames.size(); i++) {
    if (aText == aNames[i])
      return i;
  }
  if (aText == "false" && !aNames.empty())
    return 0;
  if (aText == "true" && aNames.size() > 1)
    return 1;
  if (!aText.empty() && aText.find_first_not_of("0123456789") == std::string::npos && aText.size() < 10) {
    const auto n = std::strtoul(aText.c_str(), nullptr, 10);
    if (n < aNames.size())
      return static_cast<uint32_t>(n);
  }
  return aFallback;
}

} // namespace ModSettings
