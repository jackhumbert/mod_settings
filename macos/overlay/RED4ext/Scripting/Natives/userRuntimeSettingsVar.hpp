#pragma once
// macOS overlay. Trimmed copy of deps/red4ext.sdk/include/RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp (jackhumbert's
// RED4ext.SDK fork, MIT): the Mac SDK has no such header, and Mod Settings subclasses user::RuntimeSettingsVar.
// Changes: the unused GetSettings()/constructor declarations (they carry Windows address patterns) are dropped, `0ui32` is `0u`, the
// structs after RuntimeSettingsVar (Bool, Float, ...) are dropped. The class body, field order and virtual order are the fork's.
// MAC: the vtable is the Itanium shape of the game's class (see the destructor below): slots after the first are +8 against Windows.

#include "RED4ext/CName.hpp"
#include "RED4ext/DynArray.hpp"
#include <RED4ext/Common.hpp>
#include <RED4ext/InstanceType.hpp> // ScriptInstance (the Windows fork headers got it through other includes)
#include <stdint.h>

namespace RED4ext
{

namespace user
{
struct ConfigVarFlags
{
    uint32_t isInPreGame : 1;            // 0x001
    uint32_t isInGame : 1;               // 0x002
    uint32_t isVisible : 1;              // 0x004
    uint32_t isPlatformSpecific : 1;     // 0x008
    uint32_t isDynamic : 1;              // 0x010
    uint32_t unk20 : 1;                  // 0x020
    uint32_t isInitialized : 1;          // 0x040
    uint32_t listHasDisplayValues : 1;   // 0x080
    uint32_t canBeRestoredToDefault : 1; // 0x100
    uint32_t isInput : 1;                // 0x200
    uint32_t isDisabled : 1;             // 0x400
};
enum class EConfigVarFlags : int
{
    isInPreGame = 0x001,
    isInGame = 0x002,
    isVisible = 0x004,
    isPlatformSpecific = 0x008,
    isDynamic = 0x010,
    unk20 = 0x020,
    isInitialized = 0x040,
    listHasDisplayValues = 0x080,
    canBeRestoredToDefault = 0x100,
    isInput = 0x200,
    isDisabled = 0x400,
};
DEFINE_ENUM_FLAG_OPERATORS(EConfigVarFlags);

enum class EConfigVarType : char
{
    Bool = 0,       // bool
    Int = 1,        // int
    Float = 2,      // float
    Name = 3,       // name
    IntList = 4,    // int_list
    FloatList = 5,  // float_list
    StringList = 6, // string_list
    NameList = 7,   // name_list
};
enum class EConfigVarUpdatePolicy : char
{
    Disabled = 0,                   // update_disabled
    Immediately = 1,                // immediately
    ConfirmationRequired = 2,       // require_confirmation
    RestartRequired = 3,            // require_restart
    LoadLastCheckpointRequired = 4, // require_load_last_checkpoint
};
enum class EConfigVarImportPolicy : char
{
    ReadValue = 0x0, // read_value
    Ignore = 0x1     // ignore
};
// enum class EConfigVarUnk : short
//{
//     Unk0000 = 0x0000,
//     Backend1 = 0x0001,
//     Scripts = 0x0002,
//     Backend3 = 0x0003,
//     Default = 0xFFFF
// };

// struct RuntimeSettingsVar;
//
// struct RuntimeSettingsVar_vtbl
//{
//     void*(__fastcall * sub_00)(RuntimeSettingsVar*);
//     void(__fastcall* Deinitialize)(RuntimeSettingsVar*, uint64_t);
//     bool(__fastcall* WasModifiedSinceLastSave)(RuntimeSettingsVar*);
//     bool(__fastcall* HasRequestedValue)(RuntimeSettingsVar*);
//     bool(__fastcall* HasDefaultValue)(RuntimeSettingsVar*);
//     void(__fastcall* RestoreDefault)(RuntimeSettingsVar*);
//     void(__fastcall* SetRequestedValue)(RuntimeSettingsVar*, int*);
//     void(__fastcall* sub_38)();
//     void(__fastcall* sub_40)();
//     void(__fastcall* sub_48)();
//     void(__fastcall* sub_50)();
// };

// InGameConfig::Var
#pragma pack(push, 1)
struct RuntimeSettingsVar
{
    // vft RVA: 0x32BB5C8
    // str RVA: 0x32BB408
    // Entity/VisualController
    /// @pattern 45 6E 74 69 74 79 2F 56 69 73 75 61 6C 43 6F 6E 74 72 6F 6C 6C 65 72
    /// @offset -448
    // static constexpr const uintptr_t VFT = userRuntimeSettingsVar_VFT_Addr;

    RuntimeSettingsVar()
    {
        displayNameKeys = RED4ext::DynArray<RED4ext::CName>(new RED4ext::Memory::DefaultAllocator());
        unk44 = 0xFF;
        unk45 = 0xFF;
        bitfield = ConfigVarFlags{}; // Windows spelling: `(ConfigVarFlags)0ui32`, an MSVC cast from an integer to the bit-field struct
    }

    virtual RED4ext::Memory::IAllocator* __fastcall GetAllocator(RuntimeSettingsVar*) // 00
    {
        return new Memory::DefaultAllocator();
    }
    // MAC: the fork's `Deinitialize(bool shouldFree)` (Windows slot 0x08) is MSVC's scalar deleting destructor: one slot. Itanium gives a
    // virtual destructor TWO slots (complete 0x08, deleting 0x10) and every virtual after it moves up by 8. Evidence (R41c, read-only,
    // tools/addr-match): the script natives of userSettingsVar that call a RuntimeSettingsVar virtual through [this+0x40] use
    //   Windows 0x21D8E1C slot 0x10, 0x21D8998 slot 0x18, 0x21D8A68 slot 0x28 with dl=2  (WasModified, HasChange, RestoreDefault(2))
    //   Mac     0x489D038 slot 0x18, 0x489D078 slot 0x20, 0x489D0B8 slot 0x30 with w1=2  (the same three, each +8).
    // The body keeps the fork's behaviour: release the display-key array, never free the object itself.
    virtual ~RuntimeSettingsVar()
    {
        if (displayNameKeys.capacity)
        {
            displayNameKeys.Clear();
        }
    }
    virtual bool __fastcall WasModifiedSinceLastSave() = 0; // Mac 18, Windows 10
    virtual bool __fastcall HasChange() = 0; // Mac 20, Windows 18 (HasRequestedValue)
    virtual bool __fastcall IsDefault() = 0; // Mac 28, Windows 20 (HasDefaultValue)
    virtual bool __fastcall RestoreDefault(uint8_t) = 0; // Mac 30, Windows 28
    virtual void __fastcall SetRequestedValue(void* value) = 0; // Mac 38, Windows 30 (InternalSetRequestedValue)
    virtual void __fastcall AcceptChange() = 0; // Mac 40, Windows 38 (InternalAcceptValue)
    virtual void __fastcall RejectChange() = 0; // Mac 48, Windows 40 (InternalRejectValue)
    virtual void __fastcall MarkAsSaved() = 0; // Mac 50, Windows 48 (InternalMarkAsSaved)
    virtual void __fastcall LoadValue(void* value) = 0; // Mac 58, Windows 50 (InternalLoadValue)

    CName name = CName();            // 08
    CName groupPath = CName();       // 10
    CName displayName = CName();     // 18
    DynArray<CName> displayNameKeys; // 20
    CName description = CName();     // 30
    CName warning = CName();         // 38 new in 2.3, messed up all other struct sizes, but MS doesn't use those
    EConfigVarType type;             // 38
    EConfigVarUpdatePolicy updatePolicy = EConfigVarUpdatePolicy::Immediately;
    EConfigVarImportPolicy importPolicy = EConfigVarImportPolicy::ReadValue;
    uint8_t unk3B;                  // 43
    ConfigVarFlags bitfield;        // 44
    uint32_t order;                 // 48
    // last update flag
    uint8_t unk44;                  // 4C
    // assigned to in init
    uint8_t unk45;                  // 4D
    uint8_t unk46;                  // 4E
    uint8_t unk47;                  // 4F
    uint32_t unk50_new;             // 50 new in 2.3, maybe?
};
// RED4EXT_ASSERT_SIZE(RuntimeSettingsVar, 0x48);
RED4EXT_ASSERT_SIZE(RuntimeSettingsVar, 0x54);
// char (*__kaboom)[sizeof(RuntimeSettingsVar)] = 1;
#pragma pack(pop)

} // namespace user
} // namespace RED4ext
