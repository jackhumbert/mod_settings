#pragma once

#include "Variable.hpp"
#include <RED4ext/InstanceType.hpp>
#include <RED4ext/RTTITypes.hpp>
#include "ScriptDefinitions/ScriptDefinitions.hpp"
#include <RED4ext/Scripting/Natives/Generated/EInputKey.hpp>

namespace ModSettings {

// In-memory bit order. Save reorders these into the bundle's flags word (bundle bit in the comments).
struct CScriptedDataPropertyFlags {
  uint16_t isImported : 1;         // 0x0001 - IsImported(); redscript: is_native
  uint16_t isEditable : 1;         // 0x0002
  uint16_t isInline : 1;           // 0x0004
  uint16_t isConst : 1;            // 0x0008
  uint16_t isReplicated : 1;       // 0x0010
  uint16_t isInstanceEditable : 1; // 0x0040
  uint16_t isPersistent : 1;       // 0x0100
  uint16_t isTestOnly : 1;         // 0x0200
  uint16_t isBrowsable : 1;        // 0x0400 - set by the constructor
  uint16_t unk200 : 1;             // 0x0800
  uint16_t unk400 : 1;             // not serialized, cleared by the constructor
  uint16_t hasHint : 1;            // 0x2000 - gates CScriptedDataProperty::hint
  uint16_t unk1000 : 4;
};
RED4EXT_ASSERT_SIZE(CScriptedDataPropertyFlags, 0x2);

inline void trim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }));
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

struct CScriptedDataProperty : IScriptDataObject {
  bool IsValid() const {
    return this->attributes.Get("ModSettings.mod");
  }

  RED4ext::CBaseRTTIType * GetType() const {
    return RED4ext::CRTTISystem::Get()->GetType(this->type->name);
  }

  void FromString(RED4ext::ScriptInstance pointer, const RED4ext::CString& str) const {
    RED4ext::RawBuffer buffer;
    buffer.data = (void*)str.c_str();
    buffer.size = str.Length();
    this->GetType()->FromString(pointer, buffer);
  }

  // ReadProperty(CName)

  template <typename T = RED4ext::CName> T ReadProperty(const RED4ext::CName &name) const {
    auto str = this->attributes.Get(name);
    if (str) {
      return RED4ext::CNamePool::Add(str->c_str());
    } else {
      return 0LLU;
    }
  }

  template <> std::string ReadProperty(const RED4ext::CName &name) const {
    auto str = this->attributes.Get(name);
    if (str) {
      return std::string(str->c_str());
    } else {
      return std::string();
    }
  }

  template <> uint32_t ReadProperty(const RED4ext::CName &name) const {
    auto str = this->attributes.Get(name);
    if (str) {
      uint32_t value;
      RED4ext::RawBuffer buffer;
      buffer.data = (void*)str->c_str();
      buffer.size = str->length;
      RED4ext::CRTTISystem::Get()->GetType("Uint32")->FromString(&value, buffer);
      return value;
    } else {
      return 0;
    }
  }

  template <> int32_t ReadProperty(const RED4ext::CName &name) const {
    auto str = this->attributes.Get(name);
    if (str) {
      int32_t value;
      RED4ext::RawBuffer buffer;
      buffer.data = (void*)str->c_str();
      buffer.size = str->length;
      RED4ext::CRTTISystem::Get()->GetType("Int32")->FromString(&value, buffer);
      return value;
    } else {
      return 0;
    }
  }

  ModSettingDependency* ReadDependency(const RED4ext::CName scriptClass) {
    auto dependency = (ModSettingDependency *)calloc(sizeof(ModSettingDependency), sizeof(char*));
    auto str = this->attributes.Get("ModSettings.dependency");
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
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      this->FromString(pointer, *str);
    }
  }

  template <> void ReadProperty<uint32_t>(const RED4ext::CName &name, uint32_t *pointer) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      RED4ext::RawBuffer buffer;
      buffer.data = (void*)str->c_str();
      buffer.size = str->length;
      RED4ext::CRTTISystem::Get()->GetType("Uint32")->FromString(pointer, buffer);
    }
  }

  template <> void ReadProperty<int32_t>(const RED4ext::CName &name, int32_t *pointer) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      RED4ext::RawBuffer buffer;
      buffer.data = (void*)str->c_str();
      buffer.size = str->length;
      RED4ext::CRTTISystem::Get()->GetType("Int32")->FromString(pointer, buffer);
    }
  }

  // not sure this is needed
  // template <> void ReadProperty<RED4ext::EInputKey>(const RED4ext::CName &name, int32_t *pointer) const {
  //   auto str = this->attributes.Get(name);
  //   if (str && pointer) {
  //     RED4ext::RawBuffer buffer;
  //     buffer.data = (void*)str->c_str();
  //     buffer.size = str->length;
  //     RED4ext::CRTTISystem::Get()->GetType("EInputKey")->FromString(pointer, buffer);
  //   }
  // }

  template <> void ReadProperty<float>(const RED4ext::CName &name, float *pointer) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      RED4ext::RawBuffer buffer;
      buffer.data = (void*)str->c_str();
      buffer.size = str->length;
      RED4ext::CRTTISystem::Get()->GetType("Float")->FromString(pointer, buffer);
    }
  }

  template <> void ReadProperty<RED4ext::CName>(const RED4ext::CName &name, RED4ext::CName *pointer) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      *pointer = RED4ext::CNamePool::Add(str->c_str());
    }
  }

  template <> void ReadProperty<RED4ext::CString>(const RED4ext::CName &name, RED4ext::CString *pointer) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      *pointer = *str;
    }
  }

  template <> void ReadProperty<char *>(const RED4ext::CName &name, char ** pointer) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      std::strcpy(*pointer, str->c_str());
    }
  }

  // ReadProperty(CName, &, fallback)

  template <typename T> void ReadProperty(const RED4ext::CName &name, T *pointer, const T fallback) const {
    auto str = this->attributes.Get(name);
    if (str && pointer) {
      this->FromString(pointer, *str);
    } else if (pointer) {
      *pointer = fallback;
    }
  }

  template <>
  void ReadProperty<RED4ext::CName>(const RED4ext::CName &name, RED4ext::CName *pointer, const RED4ext::CName fallback) const {
    auto str = this->attributes.Get(name);
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
  }

  RED4ext::CProperty *rttiProperty;                              // 40
  const IScriptDataObject *parent;                               // 48 - GetParent(), the owning class
  CScriptedDataPropertyFlags flags;                              // 50
  RED4ext::DynArray<RED4ext::CString> defaultNames;              // 58 - serialized as name/value pairs with defaultValues
  RED4ext::DynArray<RED4ext::CString> defaultValues;             // 68
  uint32_t unk78;                                                // 78 - zeroed by the constructor, not serialized
  uint32_t unk7C;                                                // 7C - untouched by the constructor and Save
  EVisibility visibility;                                        // 80
  RED4ext::HashMap<RED4ext::CName, RED4ext::CString> attributes; // 88 - GetAttributeValue()
  const CScriptedDataTypeRef *type;                              // B8
  RED4ext::CName hint;                                           // C0 - serialized when flags.hasHint
};
RED4EXT_ASSERT_SIZE(CScriptedDataProperty, 0xC8);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, parent, 0x48);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, flags, 0x50);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, defaultValues, 0x68);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, visibility, 0x80);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, attributes, 0x88);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, type, 0xB8);
RED4EXT_ASSERT_OFFSET(CScriptedDataProperty, hint, 0xC0);

}