#include <RED4ext/Common.hpp>
#include "ScriptDefinitions/ScriptDefinitions.hpp"

namespace ModSettings {

struct CScriptedDataFileInfo;
struct CScriptedDataEnum;
struct CScriptedDataBitfield;

// Game 2.5 layout, from CScriptedDataEnvironment's Add*/Find*/Clear functions.
struct CScriptedDataEnvironment {
  bool unk00;                                                              // 00 - when set, AddTypeRefInternal marks new type refs isExternal
  RED4ext::HashMap<RED4ext::CName, IScriptDataObject *> namedTypes;        // 08 - classes, enums, bitfields (FindClass, FindEnum)
  RED4ext::HashMap<RED4ext::CName, CScriptedDataFunction *> globalFunctions; // 38 - FindFunction
  RED4ext::HashMap<RED4ext::CName, CScriptedDataTypeRef *> typeRefsByName; // 68 - AddTypeRefInternal
  RED4ext::DynArray<CScriptedDataFileInfo *> files;                        // 98 - AddFileInfo
  RED4ext::DynArray<CScriptedDataFunction *> functions;                    // A8
  RED4ext::DynArray<CScriptedDataEnum *> enums;                            // B8 - AddEnum
  RED4ext::DynArray<CScriptedDataBitfield *> bitfields;                    // C8 - AddBitfield
  RED4ext::DynArray<CScriptedDataClass *> classes;                         // D8 - AddClass
  RED4ext::DynArray<CScriptedDataTypeRef *> typeRefs;                      // E8 - AddTypeRefInternal
  // Not mapped past here: Clear() empties containers at F8 and 168 with the same helper,
  // a red::SpinLock at 138 guards the type refs, and Clear() also touches 1A4 and 1D0.
};
RED4EXT_ASSERT_OFFSET(CScriptedDataEnvironment, namedTypes, 0x08);
RED4EXT_ASSERT_OFFSET(CScriptedDataEnvironment, typeRefsByName, 0x68);
RED4EXT_ASSERT_OFFSET(CScriptedDataEnvironment, files, 0x98);
RED4EXT_ASSERT_OFFSET(CScriptedDataEnvironment, classes, 0xD8);
RED4EXT_ASSERT_OFFSET(CScriptedDataEnvironment, typeRefs, 0xE8);

}
