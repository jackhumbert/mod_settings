#pragma once

#include "Variable.hpp"
#include "Utils.hpp"
#ifdef __APPLE__
#include "MacShims.hpp"
#include <cctype>
#endif
#include <RED4ext/InstanceType.hpp>
#include <RED4ext/RTTITypes.hpp>
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include <RED4ext/Scripting/Natives/Generated/EInputKey.hpp>

namespace ModSettings {

struct ScriptPropertyFlags {
  uint16_t isNative : 1;
  uint16_t isEditable : 1;
  uint16_t isInline : 1;
  uint16_t isConst : 1;
  uint16_t isReplicated : 1;
  uint16_t hasHint : 1;
  uint16_t isInstanceEditable : 1;
  uint16_t hasDefault : 1;
  uint16_t isPersistent : 1;
  uint16_t isTestOnly : 1;
  uint16_t isMutable : 1;
  uint16_t isBrowsable : 1;
};

inline void trim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

struct ScriptProperty : ScriptDefinition {
  bool IsValid() const {
    return this->runtimeProperties.Get("ModSettings.mod");
  }

  RED4ext::CBaseRTTIType * GetType() const {
    return RED4ext::CRTTISystem::Get()->GetType(this->type->name);
  }

  void FromString(RED4ext::ScriptInstance pointer, const RED4ext::CString& str) const {
#ifdef __APPLE__
    auto *type = this->GetType();
    if (type && type->GetName() == RED4ext::CName("Bool")) {
      // R43 (macOS): a Bool default came out false for every LTBF switch whose script default is true (all flight modes off, the
      // flight HUD off), so the car got no flight force. Parse the usual spellings here; anything else still goes to the game.
      std::string text(str.c_str());
      trim(text);
      for (auto &ch : text)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
      const bool known = text == "true" || text == "1" || text == "false" || text == "0";
      if (known)
        *reinterpret_cast<bool *>(pointer) = text == "true" || text == "1";
      else
        Utils::FromString(type, pointer, str);
      if (ModSettingsMac::DiagEnabled())
        ModSettingsMac::Log("[diag] Bool %s default text '%s' -> %d (%s)", this->name.ToString(), str.c_str(),
                            *reinterpret_cast<bool *>(pointer) ? 1 : 0, known ? "parsed" : "game parser");
      return;
    }
    Utils::FromString(type, pointer, str);
#else
    RED4ext::RawBuffer buffer;
    buffer.data = (void*)str.c_str();
    buffer.size = str.Length();
    this->GetType()->FromString(pointer, buffer);
#endif
  }

  // ReadProperty(CName)

  template <typename T = RED4ext::CName> T ReadProperty(const RED4ext::CName &name) const {
    auto str = this->runtimeProperties.Get(name);
    if (str) {
      return RED4ext::CNamePool::Add(str->c_str());
    } else {
      return 0LLU;
    }
  }

  template <> std::string ReadProperty(const RED4ext::CName &name) const {
    auto str = this->runtimeProperties.Get(name);
    if (str) {
      return std::string(str->c_str());
    } else {
      return std::string();
    }
  }

  template <> uint32_t ReadProperty(const RED4ext::CName &name) const {
    auto str = this->runtimeProperties.Get(name);
    if (str) {
      uint32_t value;
      Utils::FromString(RED4ext::CRTTISystem::Get()->GetType("Uint32"), &value, *str);
      return value;
    } else {
      return 0;
    }
  }

  template <> int32_t ReadProperty(const RED4ext::CName &name) const {
    auto str = this->runtimeProperties.Get(name);
    if (str) {
      int32_t value;
      Utils::FromString(RED4ext::CRTTISystem::Get()->GetType("Int32"), &value, *str);
      return value;
    } else {
      return 0;
    }
  }

  ModSettingDependency* ReadDependency(const RED4ext::CName scriptClass) {
    auto dependency = (ModSettingDependency *)calloc(sizeof(ModSettingDependency), sizeof(char*));
    auto str = this->runtimeProperties.Get("ModSettings.dependency");
    if (str) {
      std::string depends(str->c_str());
      trim(depends);
      if (!depends.empty()) {
        auto period = depends.find(".");
        auto equals = depends.find("=");
        if (equals != std::string::npos) {
          auto value = depends.substr(equals, depends.length());
          trim(value);
          if (value.length()) {
            dependency->value = value;
          }
        } else {
          equals = depends.length();
          dependency->value = "true";
        }
        if (period != std::string::npos) {
          auto className = depends.substr(0, period);
          auto propertyName = depends.substr(period+1, equals);
          trim(className);
          trim(propertyName);
          dependency->propertyName = RED4ext::CNamePool::Add(propertyName.c_str());
          dependency->className = RED4ext::CNamePool::Add(className.c_str());
        } else {
          dependency->className = scriptClass;
          dependency->propertyName = RED4ext::CNamePool::Add(depends.c_str());
        }
      }
    }
    return dependency;
  }

  // ReadProperty(CName, &)

  template <typename T> void ReadProperty(const RED4ext::CName &name, T *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      this->FromString(pointer, *str);
    }
  }

  template <> void ReadProperty<uint32_t>(const RED4ext::CName &name, uint32_t *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      Utils::FromString(RED4ext::CRTTISystem::Get()->GetType("Uint32"), pointer, *str);
    }
  }

  template <> void ReadProperty<int32_t>(const RED4ext::CName &name, int32_t *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      Utils::FromString(RED4ext::CRTTISystem::Get()->GetType("Int32"), pointer, *str);
    }
  }

  // not sure this is needed
  // template <> void ReadProperty<RED4ext::EInputKey>(const RED4ext::CName &name, int32_t *pointer) const {
  //   auto str = this->runtimeProperties.Get(name);
  //   if (str && pointer) {
  //     RED4ext::RawBuffer buffer;
  //     buffer.data = (void*)str->c_str();
  //     buffer.size = str->length;
  //     RED4ext::CRTTISystem::Get()->GetType("EInputKey")->FromString(pointer, buffer);
  //   }
  // }

  template <> void ReadProperty<float>(const RED4ext::CName &name, float *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      Utils::FromString(RED4ext::CRTTISystem::Get()->GetType("Float"), pointer, *str);
    }
  }

  template <> void ReadProperty<RED4ext::CName>(const RED4ext::CName &name, RED4ext::CName *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      *pointer = RED4ext::CNamePool::Add(str->c_str());
    }
  }

  template <> void ReadProperty<RED4ext::CString>(const RED4ext::CName &name, RED4ext::CString *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      *pointer = *str;
    }
  }

  template <> void ReadProperty<char *>(const RED4ext::CName &name, char ** pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      std::strcpy(*pointer, str->c_str());
    }
  }

  // ReadProperty(CName, &, fallback)

  template <typename T> void ReadProperty(const RED4ext::CName &name, T *pointer, const T fallback) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      this->FromString(pointer, *str);
    } else if (pointer) {
      *pointer = fallback;
    }
  }

  template <>
  void ReadProperty<RED4ext::CName>(const RED4ext::CName &name, RED4ext::CName *pointer, const RED4ext::CName fallback) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      *pointer = RED4ext::CNamePool::Add(str->c_str());
    } else if (pointer && fallback) {
      *pointer = fallback;
    }
  }

  void ReadDefaultValue(RED4ext::ScriptInstance pointer) const {
    if (this->defaultValues.size) {
      this->FromString(pointer, this->defaultValues[0]);
    }
#ifdef __APPLE__
    else if (ModSettingsMac::DiagEnabled()) {
      ModSettingsMac::Log("[diag] %s (%s): no default text in the script definition", this->name.ToString(),
                          this->type ? this->type->name.ToString() : "?");
    }
#endif
  }

  RED4ext::CProperty *rttiProperty;
  ScriptDefinition *parent;
  ScriptPropertyFlags flags;
  uint16_t unk2A[3];
  RED4ext::DynArray<RED4ext::CString> defaultClasss;
  RED4ext::DynArray<RED4ext::CString> defaultValues;
  uint64_t unk50;
  EVisibility unk58;
  uint8_t unk59[7];
  RED4ext::HashMap<RED4ext::CName, RED4ext::CString> runtimeProperties;
  ScriptType *type;
};
#ifdef __APPLE__
static_assert(offsetof(ScriptProperty, runtimeProperties) == 0x88);
static_assert(offsetof(ScriptProperty, type) == 0xB8); // Mac 2.5 ValidateProperty 0x11a8f30: ldr x20, [x1, #0xb8]
#else
RED4EXT_ASSERT_SIZE(ScriptProperty, 0x98);
RED4EXT_ASSERT_OFFSET(ScriptProperty, runtimeProperties, 0x60);
#endif

}