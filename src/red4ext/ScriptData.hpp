#include <RED4ext/Common.hpp>
#include "ScriptDefinitions/ScriptDefinitions.hpp"

namespace ModSettings {

struct ScriptData {
  RED4ext::HashMap<uint64_t, uint64_t> unk00;
  RED4ext::HashMap<uint64_t, uint64_t> unk30;
  RED4ext::HashMap<uint64_t, uint64_t> unk60;
  RED4ext::DynArray<void *> files;
  RED4ext::DynArray<void *> functions;
  RED4ext::DynArray<void *> enums;
  RED4ext::DynArray<void *> unkC0;
  RED4ext::DynArray<ScriptClass *> classes;
  RED4ext::DynArray<void *> types;
  RED4ext::DynArray<RED4ext::CString> strings;
  RED4ext::HashMap<uint64_t, uint64_t> unk100;
  uint8_t unk60MUTX;
  void *unk138;
};

}