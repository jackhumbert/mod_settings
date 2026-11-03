# Mod Settings on macOS: layouts checked against the Mac binary (R41c-2)

What `ModSettings::ProcessScriptData` (the `ReleaseScriptData` hook) and the override path read from game memory, and the evidence that the Mac
game has it where the plugin's structs say. Offsets are bytes. "Mac" addresses are RVAs in `Cyberpunk2077`; tools are `tools/addr-match/rx`
and small scripts over `rx.py` (read-only, no game running). Windows offsets are the plugin's own (the ones its `RED4EXT_ASSERT_*` assert).

## 1. ScriptBundle (the object ReleaseScriptData receives)

The plugin's `ModSettings::ScriptData` (`src/red4ext/ScriptData.hpp`), checked against the Mac ScriptBundle destructor `0x21ECFD8` (Windows
`0x7B7C18`), which destroys the members in reverse declaration order:

| plugin field | offset | Mac destructor `0x21ECFD8` | status |
|---|---|---|---|
| unk138 (a 40-byte object) | 0x138 | `add x0,x19,#0x138; bl 0x4A3E770` is the first member destroyed | same |
| unk100 `HashMap` | 0x100 | `add x0,x19,#0x100; bl 0x1614D04` | same |
| strings `DynArray<CString>` | 0xF0 | capacity `[+0xF8]`, size `[+0xFC]`, element stride 0x20 (`lsl #5`), `CString` dtor `bl 0x29228` | same, `CString` is 0x20 |
| types `DynArray` | 0xE0 | capacity `[+0xE8]`, size `[+0xEC]`, stride 8 | same |
| classes `DynArray<ScriptClass*>` | 0xD0 | capacity `[+0xD8]`, size `[+0xDC]`, stride 8 | same (also read by Codeware's CheckLayout every boot) |
| unkC0 / enums / functions / files `DynArray` | 0xC0 / 0xB0 / 0xA0 / 0x90 | the same 5-instruction block at each, in that order | same |
| 3 `HashMap`s | 0x60 / 0x30 / 0x00 | each: size `[+8]`, capacity `[+0xC]`, index table `[+0]`, node array at `+0x10` | same; `HashMap` is 0x30 on both |

`DynArray` is `{entries +0, capacity +8, size +0xC}` and `HashMap` is `{index table +0, size +8, capacity +0xC, nodes +0x10}` on the Mac, which is
what the SDK types the plugin compiles against assume. The plugin never constructs or copies a bundle, so its 0x140-byte model (the real object is
0x160) only matters for the offsets above.

## 2. ScriptClass

| field | plugin | evidence |
|---|---|---|
| vtable | `ScriptDefinition` virtuals | Mac vtable `0x70006E8`: slots `GetAllocator, ~D1, ~D0, GetType (mov w0,#1), GetVisibility, GetParent, ..., IsNative`: two destructor slots as in the SDK's Mac header |
| `name` | +0x08 | Codeware CheckLayout reads it and checks it is a printable CName, every boot |
| `parent` | +0x20 | slot 6 (0x30) of the vtable above is `ldr x0,[x0,#0x20]; ret` (slot 5 is `mov x0,#0; ret`); Codeware reads the field |
| `properties` `DynArray<ScriptProperty*>` | +0x28 | Codeware reads capacity `+0x30` and size `+0x34` |
| `visibility` | +0x88 | `GetVisibility` of the vtable above is `ldrb w0,[x0,#0x88]; ret` |
| `flags` | +0x89 | `IsNative` is `ldrb w8,[x0,#0x89]; and w0,w8,#1; ret`; Codeware reads the same byte |

`ProcessScriptData` reads `name`, `properties` and, through `ToClass(name)`, the RTTI class. It does not read `rttiType`, `parent` or `flags`.

## 3. ScriptProperty

| field | plugin | evidence |
|---|---|---|
| vtable | | Mac vtable `0x7000868` (`GetType` = `mov w0,#8; ret`) |
| `parent` | +0x20 | slot 5 (0x28) is `ldr x0,[x0,#0x20]; ret` (`GetParent`; slot 6 is `mov x0,#0; ret`). `prop->parent->name` is the class name of a variable |
| `flags` | +0x28 (uint16) | `IsNative` is `ldrh w8,[x0,#0x28]; and w0,w8,#1; ret` |
| `defaultClasss` `DynArray<CString>` | +0x30 | the Mac destructor `0x21FA298` (the body behind `~D1` / `~D0` `0x21F9828` / `0x21F982C`): capacity `[+0x38]`, size `[+0x3C]`, stride 0x20 |
| `defaultValues` `DynArray<CString>` | +0x40 | same destructor: capacity `[+0x48]`, size `[+0x4C]`, stride 0x20. `ReadDefaultValue` takes element 0 |
| `visibility` | +0x58 | `GetVisibility` is `ldrb w0,[x0,#0x58]; ret` |
| `runtimeProperties` `HashMap<CName, CString>` | +0x60 | the destructor's first call: `str x8,[x0],#0x60; bl 0x94DD44` destroys the `HashMap` at `+0x60`, then the arrays at `+0x40` and `+0x30` (reverse declaration order) |
| `type` `ScriptType*` | +0x90 | the Mac `ScriptValidator::ValidateProperty` `0x21FCFB8` starts `ldr x20,[x1,#0x90]` |
| `ScriptType.name` | +0x08 | Codeware reads it (`NameStr(prop->type->name)`), same field as every `ScriptDefinition` |

`ProcessScriptData` reads `prop->runtimeProperties` (`.size`, `.Get(CName)`), `prop->name`, `prop->parent->name`, `prop->type->name`,
`prop->defaultValues[0]` and the plugin's `ReadProperty` helpers on the attribute strings. `HashMap<CName, CString>::Get` is the same SDK template as the
`HashMap<CName, ScriptDefinition*>` that Codeware's guard calls on the bundle every boot (only the value stride differs).
Not read and not checked: `rttiProperty` (+0x18), `ScriptType.innerType` / `metaType`.

## 4. user::RuntimeSettingsVar (subclassed by the plugin, called by the game)

The Mac vtable is the Windows one with every slot after the first +8 (a virtual destructor takes two slots). Windows natives
`0x21D8E1C / 0x21D8998 / 0x21D8A68` call slots `0x10 / 0x18 / 0x28` (the last with `dl = 2`); the Mac twins
`0x489D038 / 0x489D078 / 0x489D0B8` call `0x18 / 0x20 / 0x30` (the last with `w1 = 2`). `macos/overlay/RED4ext/Scripting/Natives/userRuntimeSettingsVar.hpp`
declares the destructor, and a compiled probe gives `RuntimeVariableBool` slots 0x18 .. 0x58. `inkActionName.action` is a CName at +0x40
(`layout/reflection-2026-10-08.tsv`), the field the fork calls `name`. Slot 0 (`GetAllocator`) is not verified.

## 5. The override path (R41b, `let_there_be_flight macos/ADDRESS_LEFTOVERS.md`)

Verified as on Windows: `UserMapping.rawMappings` (a `Map<CName, RawMappingCollection>` at +0x98), `RawMappingCollection` (stride 0x18, `needs_update` at +0x10),
`Mapping` (stride 0x30, `overridableUI` at +0x24), `ContextManager.userMapping` (+8) and `.actions` (+0x28). Not verifiable statically: the back pointer
`UserMapping.contextManager` (+0x48). `MacUserMappingUsable` checks it once per call without faulting (`vm_read_overwrite`):
`userMapping->contextManager->userMapping == userMapping`, else the override feature is skipped for that mapping with one log line.

## 6. Not checked

- `ScriptProperty.rttiProperty`, `ScriptType.innerType` / `metaType`, `ScriptClass.rttiType` / `flags`: the plugin does not read them.
- Slot 0 of the `RuntimeSettingsVar` vtable, and whether the game calls its destructors on Mod Settings' objects.
- Everything only a running game can show (the first menu boot with Mod Settings loaded).

Compile-time sizes the plugin gets for the Mac (a scratch compile probe, not part of the repo): `ScriptClass` 0x90, `ScriptProperty` 0x98 with `runtimeProperties` at
0x60 and `type` at 0x90, `IScriptable` 0x40, `ModConfigVar<T>::runtimeVar` at 0x40, `user::SettingsVar` 0x48, `Map` 0x28, `HashMap` 0x30, `DynArray` 0x10, `CString` 0x20.
