#include <RED4ext/Common.hpp>
#include "ScriptDefinitions/ScriptDefinitions.hpp"

namespace ModSettings {

struct ScriptData {
#ifdef __APPLE__
  void *unk00_25; // game 2.5: 8 new bytes at the start (definitionsByName +8, classes +0xD8)
#endif
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
#ifdef __APPLE__
static_assert(offsetof(ScriptData, classes) == 0xD8);
#endif

}