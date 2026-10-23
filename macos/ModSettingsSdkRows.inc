// R41c. Mac RVAs for the address hashes Mod Settings uses that the Mac SDK table (RED4ext.SDK-macos, RED4ext/Relocation-inl.hpp,
// macOSAddressTable) does not have. CMakeLists.txt injects this file after the CGlobalFunction_ctor row of a generated copy of
// that header, so the SDK's own UniversalRelocFunc calls and ModModuleHookHash (macos/overlay/Registrar.hpp) resolve them.
// Format: { hash, RVA },  // name. RVA is relative to the Cyberpunk2077 image base (Mac RVA = address - 0x100000000).
// Evidence: ltbf/let_there_be_flight/macos/ADDRESS_SURVEY.md (R40, "Mod Settings hooks") and tools/addr-match.
//
// Never a 0 RVA: an unmapped hash goes to the trap (macos/MacTrap.cpp), and a hook on a trap is refused.

// ---- Mod Settings' own hooks and calls (R41b, let_there_be_flight macos/ADDRESS_LEFTOVERS.md) ----------------------------------------------------------------
        { 3993832650, 0x111CC00 },  // ReleaseScriptData = ScriptBundle_dtor. void(ScriptBundle*). ALREADY a Codeware row: cp2077-codeware/macos/MacAddressResolver.cpp line 29, "ScriptBundle_dtor ... Windows 0x7B7C18: same member destruction order (+0x138 ... +0x90)". Codeware HOOKS this target too (cp2077-codeware/macos/hook-offsets.txt, HookAfter<ScriptBundle::Destruct>), so Mod Settings joins Codeware's hook chain (MacShims.cpp AttachHook). R41c-2 re-read the Mac destructor: members from +0x138 down to +0x90 as in macos/LAYOUT.md.
        { 1773342692, 0x30A0D5C },  // ApplyOverrides BODY. void(UserMapping*). R41b CONFIRMED: a real entry (in LC_FUNCTION_STARTS, [0x1D792E4, 0x1D795A8), standard frame, no PC-relative instruction in its first 4 words); Windows 0x4A61C4. Windows hooks the 14-byte thunk 0x1868498 (Manager*); the Mac thunks 0x1D735A4 (8 bytes) and 0x1D5C774 are too short for an entry hook. The body gets only the UserMapping*.
        { 3702071356, 0x30A1114 },  // UserMapping::GetMappingCollection. RawMappingCollection*(UserMapping*, CName): this in x0, the CName hash in x1. R41b MAPPED, Windows 0x4A60D0 (callers paired: Win 0x4A5ED0 = Mac 0x1D5C02C, STRONG).
        { 4196866905, 0x30A0CB8 },  // UserMapping::ClearRuntimeDataOnRawMappings. void(UserMapping*). R41b MAPPED, Windows 0x4A6A54 (first call of the same game function on both sides); the Mac loop is vectorised.

// ---- Rows copied unchanged from Codeware's cp2077-codeware/macos/MacSdkRows.inc ---------------------------------------------------
// The SDK headers this plugin compiles (CClass, IScriptable, ISerializable, CBaseFunction) call these through UniversalRelocFunc, and
// a hash that is only in Codeware's table would resolve to a trap in this dylib. Evidence and Windows twins: see the comments of each row
// in that file (R16a, R19). Only the hashes hashscan finds embedded in mod_settings.dylib are copied.
        { 3778157291, 0x109B16C },  // CBaseFunction_ExecuteNative. bool(CBaseFunction*, CStack*): builds its own frame. Windows 0x20668A0.
        { 2547979664, 0x10C3934 },  // CClass_AssignDefaultValuesToProperties. void(CClass*, ScriptInstance). Windows InitializeScriptDefaultValues 0x147384.
        { 2257327441, 0x10C37D8 },  // CClass_InitializeProperties. void(CClass*, ScriptInstance). Windows InitializeScriptedProperties 0x147CB8.
        { 55710513,   0x11E4964 },  // IScriptable_DestructValueHolder. void(IScriptable*). Replaces the TweakXL stand-in. Windows 0x14934C.
        { 4175699423, 0x11e4da0 },  // IScriptable_sub_D8. void(this, unused, visitor). Windows 0x205FE10.
        { 114701897,  0x10b09a4 },  // ISerializable_sub_30. bool(this): true when there is no owning CResource. Shared by about 6700 vtables, never hook. Windows 0x2052B84.
        { 2293436409, 0x10B0860 },  // ISerializable_sub_40. bool(this, BaseStream*). Windows 0x148940.
        { 2715367827, 0x10B0C08 },  // ISerializable_sub_78. Mac: Handle(this, a1, a2 byte, a3, a4) with the Handle through x8 (the SDK signature models a hidden first argument). Windows 0x711A14.
        { 3978435378, 0x10b0b6c },  // ISerializable_sub_A0. The base default (GetType()->GetDefaultObject). The address table's Windows 0xB9A864 is a CResource override; the twin is 0x1247898.
        { 2149588021, 0x10b0b44 },  // ISerializable_sub_C0. The base default: the owning CResource's +0x30, returned in x0. The address table's Windows 0x81DA2C is unrelated; the twin is 0x4B0D9C.

// ---- No row on purpose ---------------------------------------------------------------------------------------------------------------
//   3298233196  SkipKey. R40 matched Windows 0x21F01C0 to Mac 0x48026F0, but R41b found no reference to it of any kind (no bl, b, adr or data
//               reference; the only caller's twin, 0x48038B8, inlines the predicate). The hook could never fire and is not installed on Mac.
//   2583235069  CalcHash(const CString&). Inlined in the Mac game (R41b); reimplemented in plugin code (Hooks/ApplyOverrides.hpp).
