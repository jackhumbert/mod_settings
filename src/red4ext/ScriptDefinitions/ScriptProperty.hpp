#pragma once

#include "ScriptDefinitions/ScriptDefinitions.hpp"

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

struct ScriptProperty : ScriptDefinition {
  bool IsValid() const {
    return this->runtimeProperties.Get("ModSettings.mod");
  }

  template <typename T> void ReadProperty(const RED4ext::CName &name, T *pointer) const {
    auto propType = RED4ext::CRTTISystem::Get()->GetType(this->type->name);
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      propType->FromString(pointer, *str);
    }
  }

  template <typename T> void ReadProperty(const RED4ext::CName &name, T *pointer, const T fallback) const {
    auto propType = RED4ext::CRTTISystem::Get()->GetType(this->type->name);
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      propType->FromString(pointer, *str);
    } else if (pointer) {
      *pointer = fallback;
    }
  }

  template <> void ReadProperty<uint32_t>(const RED4ext::CName &name, uint32_t *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      RED4ext::CRTTISystem::Get()->GetType("Uint32")->FromString(pointer, *str);
    }
  }

  template <> void ReadProperty<int32_t>(const RED4ext::CName &name, int32_t *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      RED4ext::CRTTISystem::Get()->GetType("Int32")->FromString(pointer, *str);
    }
  }

  template <> void ReadProperty<float>(const RED4ext::CName &name, float *pointer) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      RED4ext::CRTTISystem::Get()->GetType("Float")->FromString(pointer, *str);
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

  template <>
  void ReadProperty<RED4ext::CName>(const RED4ext::CName &name, RED4ext::CName *pointer, const RED4ext::CName fallback) const {
    auto str = this->runtimeProperties.Get(name);
    if (str && pointer) {
      *pointer = RED4ext::CNamePool::Add(str->c_str());
    } else if (pointer && fallback) {
      *pointer = fallback;
    }
  }

  RED4ext::CName ReadProperty(const RED4ext::CName &name) const {
    auto str = this->runtimeProperties.Get(name);
    if (str) {
      return RED4ext::CNamePool::Add(str->c_str());
    } else {
      return 0LLU;
    }
  }

  std::string ReadPropertyString(const RED4ext::CName &name) const {
    auto str = this->runtimeProperties.Get(name);
    if (str) {
      return std::string(str->c_str());
    } else {
      return std::string();
    }
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
RED4EXT_ASSERT_SIZE(ScriptProperty, 0x98);
RED4EXT_ASSERT_OFFSET(ScriptProperty, runtimeProperties, 0x60);