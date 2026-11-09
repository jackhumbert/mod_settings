#pragma once

// Parsing of the ModSettings.dependency attribute, "[Class.]property[=value]", kept apart from the engine types so
// tests/ can link it without the game.

#include <algorithm>
#include <cctype>
#include <string>

namespace ModSettings {

struct DependencyString {
  std::string className; // empty: the field's own class
  std::string propertyName;
  std::string value;     // compared with the property's ToString; "true" when the attribute gives none
};

inline void TrimSpaces(std::string &s) {
  auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
  s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
}

// false when the attribute names no property (empty, or only "=value")
inline bool ParseDependency(std::string depends, DependencyString &out) {
  out = {};
  out.value = "true";
  auto equals = depends.find('=');
  if (equals != std::string::npos) {
    out.value = depends.substr(equals + 1);
    depends.resize(equals);
    TrimSpaces(out.value);
  }
  TrimSpaces(depends);
  auto period = depends.find('.');
  if (period != std::string::npos) {
    out.className = depends.substr(0, period);
    out.propertyName = depends.substr(period + 1);
    TrimSpaces(out.className);
    TrimSpaces(out.propertyName);
  } else {
    out.propertyName = depends;
  }
  return !out.propertyName.empty();
}

} // namespace ModSettings
