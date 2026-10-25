#pragma once

#include "RED4ext/RTTITypes.hpp"
#include <RED4ext/RED4ext.hpp>

// The game's in-memory script definitions (what a script bundle is loaded into), game 2.5.
// Class names, virtuals and enums come from SHA-256 confirmed symbols in the game's
// cyberpunk2077_addresses.json. Layouts and member names come from the 2.5 constructors,
// Save/Load and the CScriptedDataEnvironment Add*/Find* functions. Members still named unk*
// are never read or written by Load/Save; what is known about them is noted inline.

namespace ModSettings {

struct IScriptDataSaver;
struct IScriptDataLoader;
struct CScriptedDataClass;
struct CScriptedDataProperty;
struct CScriptedDataTypeRef;
struct CScriptedDataFunction;

struct IScriptDataObjectFlags {
  // Set when the definition resolves to one that already exists (native, or from another
  // bundle). Save writes nothing past the flags and LoadObjects skips Load for it.
  uint8_t isExternal : 1;
  uint8_t hasUnk28 : 1;   // unk28 is serialized
  uint8_t hasUnk30 : 1;   // unk30 is serialized
  uint8_t unk08 : 1;      // set together with isExternal by CScriptDataLoader::ResolveObjects
  uint8_t unk10 : 4;
};
RED4EXT_ASSERT_SIZE(IScriptDataObjectFlags, 0x1);

struct IScriptDataObject {
  enum class EMetaType : uint8_t {
    TypeRef = 0x0,
    Class = 0x1,
    Constant = 0x2,
    Enum = 0x3,
    Bitfield = 0x4,
    Function = 0x5,
    FunctionParam = 0x6,
    FunctionLocal = 0x7,
    Property = 0x8,
    FileInfo = 0x9,
  };

  enum class EVisibility : uint8_t {
    Public = 0x0,
    Protected = 0x1,
    Private = 0x2,
  };

  virtual const RED4ext::Memory::IAllocator &GetMemoryPool() const = 0; // 00 - red::memory::Pool
  virtual ~IScriptDataObject() = default;                               // 08
  virtual EMetaType GetMetaType() const = 0;                            // 10
  virtual EVisibility GetVisibility() const = 0;                        // 18
  virtual const IScriptDataObject *GetParent() const = 0;               // 20
  virtual const IScriptDataObject *GetBase() const = 0;                 // 28
  virtual bool IsImported() const = 0;                                  // 30
  virtual void Save(IScriptDataSaver &aSaver) const = 0;                // 38
  virtual void Load(IScriptDataLoader &aLoader) = 0;                    // 40

  RED4ext::CName name;                    // 08
  uint32_t unk10;                         // 10 - runtime only
  uint8_t unk14;                          // 14 - runtime only
  uint64_t unk18;                         // 18 - runtime only
  IScriptDataObjectFlags objectFlags;     // 20 - added in 2.5
  RED4ext::CName unk28;                   // 28 - added in 2.5, serialized when hasUnk28; vanilla final.redscripts never sets it
  RED4ext::DynArray<RED4ext::CName> unk30; // 30 - added in 2.5, serialized when hasUnk30; vanilla final.redscripts never sets it
};
RED4EXT_ASSERT_SIZE(IScriptDataObject, 0x40);
RED4EXT_ASSERT_OFFSET(IScriptDataObject, name, 0x08);
RED4EXT_ASSERT_OFFSET(IScriptDataObject, objectFlags, 0x20);
RED4EXT_ASSERT_OFFSET(IScriptDataObject, unk28, 0x28);
RED4EXT_ASSERT_OFFSET(IScriptDataObject, unk30, 0x30);

struct CScriptedDataTypeRef : IScriptDataObject {
  // Values match the CScriptedDataEnvironment::AddTypeRef* functions. Enums and bitfields
  // are Class refs (AddTypeRefEnum/AddTypeRefBitfield share AddTypeRefClass's code).
  enum class EKind : uint32_t {
    Named = 0x0,
    Class = 0x1,
    Handle = 0x2,
    WeakHandle = 0x3,
    DynArray = 0x4,
    StaticArray = 0x5,
    Reference = 0x6,
  };

  RED4ext::CBaseRTTIType *rttiType;  // 40
  const IScriptDataObject *innerType; // 48 - serialized for Handle..Reference
  uint32_t staticArraySize;          // 50 - serialized for StaticArray
  EKind kind;                        // 54
};
RED4EXT_ASSERT_SIZE(CScriptedDataTypeRef, 0x58);
RED4EXT_ASSERT_OFFSET(CScriptedDataTypeRef, innerType, 0x48);
RED4EXT_ASSERT_OFFSET(CScriptedDataTypeRef, kind, 0x54);

// In-memory bit order. Save reorders these into the bundle's flags word (bundle bit in the comments).
struct CScriptedDataClassFlags {
  uint8_t isImported : 1;    // 0x001 - IsImported(); redscript: is_native
  uint8_t isImportOnly : 1;  // 0x040
  uint8_t isAbstract : 1;    // 0x002
  uint8_t isFinal : 1;       // 0x004
  uint8_t isStruct : 1;      // 0x008
  uint8_t hasAttributes : 1; // 0x080 - gates CScriptedDataClass::attributes; redscript: is_test_only
  uint8_t unk40 : 1;         // 0x200
  uint8_t unk80 : 1;         // not serialized, kept by the constructor
};
RED4EXT_ASSERT_SIZE(CScriptedDataClassFlags, 0x1);

struct CScriptedDataClass : IScriptDataObject {
  RED4ext::CClass *rttiType;                                 // 40
  const CScriptedDataClass *base;                            // 48 - GetBase()
  RED4ext::DynArray<CScriptedDataProperty *> properties;     // 50 - owned
  RED4ext::DynArray<CScriptedDataProperty *> overrides;      // 60 - owned, AddOverridenProperty()
  RED4ext::DynArray<void *> unk70;                           // 70 - added in 2.5, runtime only, not owned
  RED4ext::DynArray<void *> unk80;                           // 80 - added in 2.5, runtime only, not owned
  RED4ext::DynArray<CScriptedDataFunction *> functions;      // 90 - owned
  RED4ext::HashMap<RED4ext::CName, RED4ext::CString> attributes; // A0
  EVisibility visibility;                                    // D0
  CScriptedDataClassFlags flags;                             // D1
};
RED4EXT_ASSERT_SIZE(CScriptedDataClass, 0xD8);
RED4EXT_ASSERT_OFFSET(CScriptedDataClass, base, 0x48);
RED4EXT_ASSERT_OFFSET(CScriptedDataClass, properties, 0x50);
RED4EXT_ASSERT_OFFSET(CScriptedDataClass, functions, 0x90);
RED4EXT_ASSERT_OFFSET(CScriptedDataClass, attributes, 0xA0);
RED4EXT_ASSERT_OFFSET(CScriptedDataClass, visibility, 0xD0);
RED4EXT_ASSERT_OFFSET(CScriptedDataClass, flags, 0xD1);

}
