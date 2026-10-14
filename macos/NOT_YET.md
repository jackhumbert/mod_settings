# Mod Settings on macOS: what is not done yet (R41c, R41c-2)

Everything here is either a fail-safe (the feature is skipped with one log line, nothing jumps to a bad address) or a check that needs the
game. Build: see the header of `macos/CMakeLists.txt`. Rows: `macos/ModSettingsSdkRows.inc`. Hook allowlist: `macos/hook-offsets.txt`.
Layout evidence: `macos/LAYOUT.md`.

## 0. Registration needs a loader step that calls `modsettings_registerNatives`

On the Mac the game never calls the RTTI register callbacks on the loader setup this was developed on (Codeware:
`lib/Support/RedLib/RedLibProvider.cpp`), so the Windows `Main(Load)` work that depends on them (register the RedLib types, register the
ArchiveXL archive) runs in the exported `modsettings_registerNatives` (`macos/MacEntry.cpp`), and the hooks are attached there, late, so
that Codeware's own hooks exist.

The loader's Frida gadget script calls it at the entry of the game's InitScripts (0x3D8C188), after `tweakxl_registerNatives`,
`archivexl_registerNatives` and `codeware_registerNatives` (ArchiveXL's natives must exist before the archive is registered).
Done and tested in game (2026-10-12/13: Settings > Mods lists Let There Be Flight with 66 settings; a changed value is written to
`user.ini`, read back after a restart, and seen by the mod). jackmaxwil's public RED4ext-macos has no Frida script: there the same function
would run from the game's post-register callback (planned, not done).

## 0b. Bool defaults (fixed 2026-10-13)

Every Bool setting whose script default is `true` came out `false`: the game's Bool `FromString` did not set `true` for the default
text. `ScriptProperty::FromString` now parses `true`/`false`/`1`/`0` itself on macOS (anything else still goes to the game's parser),
and a definition with no default text gets a value-initialised default. `MODSETTINGS_MAC_DIAG=1` logs each Bool default and its result.

## 0c. FromString's text argument (fixed 2026-10-13)

The cause of 0b, and of a start-up crash: the Mac game's `FromString` (vtable slot +0x78) takes the text by value as a (pointer, 32-bit
length) pair in x2 and x3, while the Mac SDK declares `FromString(ScriptInstance, const CString&)`. Called through the SDK declaration,
x2 held the CString's address and x3 (the length) was never set. Defaults, min/max/step texts and saved values were parsed with a
garbage length: key binding defaults came out as `IK_None`, and once `user.ini` held a key binding, reading it ran the enum parser off
the end of the stack (crash at start-up). `Utils::FromString` now calls the same slot with the game's argument type. Tested in game with
a `user.ini` that crashed the previous build: it starts, and a changed value is written, read back and seen by the mod.

## 1. Key-binding overrides: live, never run in game

R41b gave the addresses: `UserMapping::GetMappingCollection` 0x1D796B4 and `ClearRuntimeDataOnRawMappings` 0x1D7923C are rows;
`CalcHash` is inlined in the Mac game, so the plugin has the loop (`h = c + 31*h` over signed chars from 0, 32-bit wrap; the arithmetic was
checked against Java's `"abc".hashCode()` = 96354). The hook is on the ApplyOverrides body 0x1D792E4 (`void(UserMapping*)`, a real entry) and is
in the default build (`-DMODSETTINGS_MAC_APPLYOVERRIDES=OFF` compiles it out; then take 0x1d792e4 out of `hook-offsets.txt`). `kMacOverrideCallsReady` is true.

- Accepted loss: the body gets only the `UserMapping*`, so `Manager::overrideSet` and `lastOverrideStaus` are not set.
  Key bindings still apply; the game's change notification for them is not raised. The vtable at 0x6F945F0 is not patched. Logged once.
- `UserMapping.contextManager` (+0x48) is the one field R41b could not verify. `MacUserMappingUsable` checks it before every override pass, without
  faulting: `contextManager->userMapping == userMapping`; otherwise the feature is skipped with one log line.
- Never run in game: that the body hook fires, that `GetMappingCollection` finds the collections, that a rebound key reaches the game.

## 2. SkipKey is not hooked on the Mac

R40 matched Windows 0x21F01C0 to Mac 0x48026F0; R41b found no reference to it of any kind, and its only caller's twin (0x48038B8) inlines the
predicate. The hook could never fire, so it is not installed and 0x48026f0 is out of `hook-offsets.txt` and the rows. Effect: gamepad-range keys
(`IK_Pad_First` .. +0x13) are not filtered out of the keyboard binding capture, as in vanilla. Logged once at start.

## 3. ReleaseScriptData shares its target with Codeware

0x21ECFD8 (ScriptBundle_dtor, hash 3993832650) is hooked by Codeware (`HookAfter<ScriptBundle::Destruct>`, installed in its Main), and RED4ext allows
one hook per target. Mod Settings attaches from `modsettings_registerNatives`, after every Main, and:

- `codeware_mac_hook_chain_attach` exists and accepts the target: `CHAINED` (the detour runs first, Codeware's runs as its "original");
- it exists and refuses: `REFUSED`, logged, no direct attach (Codeware is loaded but does not own the target);
- the symbol does not exist (Codeware not loaded): a direct RED4ext attach.

Plugin load order no longer matters: the old "Codeware loads after Mod Settings" refusal is gone. Tested offline with a fake RED4ext and a fake
chain symbol (all four outcomes plus a failed attach, a RED4ext no-op and a null target).

## 4. Only the game's bundle is processed

`ProcessScriptData` clears the mod list and rebuilds it from the bundle it gets. Codeware's test for "the real game bundle" is its
`Mac::ScriptsValidated()` flag (set after its `ScriptValidator::Validate` hook returned), which is hidden inside Codeware.dylib. Mod Settings uses
what Codeware's own `CheckLayout` accepts: `classes` (+0xD0) sane with at least one class, the first classes with a vtable in the game image, a printable
name and a plausible header, and `definitionsByName` finding `ISerializable` and `IScriptable` (`macos/MacBundle.cpp`, every read through
`vm_read_overwrite`). Any other bundle is skipped with one log line per reason and the original destructor still runs. The failure paths were run
offline (null, zeroed, misaligned and unmapped entries, a vtable outside the game image); the success path needs the game.

## 5. SDK hashes in the dylib that resolve to traps (fail-safe)

No row in the Mac SDK table or Codeware. The trap logs once on its first call and returns 0. Codeware has the same ones (`macos/NOT_YET.md`: "unreached").

| hash | name | note |
|---|---|---|
| 434901863 | `Memory_Vault_Unk1` | allocator error path |
| 3578730042 | `Memory_PoolStorage_OOM` | allocator out-of-memory path, `AllocationResult` through x0/x1 |
| 3832485534, 364779332, 2757309857, 2930319133, 2396396482 | `CBaseRTTIType_sub_80`, `_88`, `_90`, `_98`, `_A0` | `bool` virtuals of the SDK's `CBaseRTTIType`, instantiated now that the RedLib class descriptors are compiled in |

Hashes that appear in the dylib only as data words (hashscan "imm=0"): the 13 data-hash entries of the trap table in `MacTrap.cpp` (`LaunchParameters`,
`CGameEngine`, `g_DeviceData`, `CBaseFunction_Handlers`, ...) and the keys of the Mac SDK table itself; nothing resolves them.

## 6. Layouts

See `macos/LAYOUT.md`. Checked against the Mac binary: the whole ScriptBundle destructor, the ScriptClass and ScriptProperty vtables and the ScriptProperty
destructor (so every field `ProcessScriptData` reads), `ValidateProperty` (`type` at +0x90), the `RuntimeSettingsVar` vtable (+8 after the first slot).
Not checked: `ScriptProperty.rttiProperty`, `ScriptType.innerType` / `metaType` (not read), slot 0 of the `RuntimeSettingsVar` vtable and whether the game
calls its destructors, and everything only a running game shows.

## 7. RedLib: what was ported from Codeware's `lib/Red` (the Mac fixes, found with `git log -- lib/Red`)

Ported into `macos/redlib` (every changed line is marked MAC):

1. `X_RTTI_FORCE_REGISTRAR` (Codeware commit 505b778c) on every definition macro. **Without it nothing registers**: the registrar is an `inline static` member of a
   class template specialization, MSVC instantiates it eagerly and clang only when it is odr-used. The R41c dylib had no registrar at all (`nm` showed no
   `s_registrar`); now 7 type registrations and 8 descriptions are queued at load (measured by dlopen in a scratch process, `modsettings_mac_pending`).
2. `TypeInfoRegistrar::RunPending()`, `PendingRegisterCount()`, `PendingDescribeCount()` (505b778c): the plugin runs the queued callbacks itself,
   because the game never calls them (section 0).
3. `Resolving.hpp` (505b778c, c96cac0c; ported in R41c): clang's type names have no `enum ` prefix, `size_t i, j` in one declaration, `IsTypeNameConst`, `const char*` tested first.

Not ported, because Mod Settings does not use the code:

- `MacGameInstance`, `MacPublishGlobal`, `SystemBuilder` changes (game systems, global functions): the plugin defines neither.
- `Detail::InstancePtr` aliases (505b778c, 9170c502): this RedLib already spells the type virtuals `ScriptInstance`, and it compiles against the Mac SDK.
- `ClassDefinition::s_skipped` (R22, skip a class another plugin registered first) and the `ALIAS` fallback of `ClassExpansion`: the class names are the plugin's own; `inkActionName` has NAME == ALIAS.
- `typename T::ForwardAllocator` / `ForwardInitializer` concepts, `ExtractArg` ordering, `AddProperty<offset>`, `SetNamespace`, `Construction.hpp` `HasStaticAllocator`,
  `Specializations.hpp` hashes (TweakDBID offset bits, NodeRef, `uint64_t`), `Invocation.hpp` (calling script functions with arguments): upstream RedLib changes or
  features the plugin does not use (it uses `RTTI_IMPL_ALLOCATOR`, plain-value and handle parameters, and `ExecuteFunction`).
- "RawFunc by-value returns" (x8) and `Red::SretValue` are Codeware's `Core::RawFunc` layer, not RedLib. Mod Settings calls only game functions that return a
  scalar, a pointer or nothing (GetMappingCollection, ClearRuntimeData, the SDK's own rows).

## 8. Differences from Windows that are on purpose

- Types, the ArchiveXL archive and the hooks are set up in `modsettings_registerNatives`, not in `Main` (section 0). `Main(Load)` only sets up logging and logs that it waits.
  It does not call `scripts->Add` (RED4ext looked for `packed.reds` and `module.reds` at the plugin root and logged "Could not find path relative to plugin" in the first menu boot).
  `ArchiveXL::RegisterArchive` is skipped when `archive/Mac/mod/ModSettings.archive` exists (ArchiveXL mounts that folder itself at startup); the call at NATREG time is too
  late, ArchiveXL logged "depot is already initialized" in the first menu boot. Without that file it still calls it, as before.
  Mod Settings has no dependency check and `Main` always returns true, so plugin load order cannot fail it: Codeware is looked up by symbol and ArchiveXL by RTTI class
  only inside `modsettings_registerNatives`, after every `Main`, and a missing one is logged, not fatal. `ArchiveXL::RegisterArchive` uses the overlay copy of the header: the Windows readiness test
  (an engine pool in the vault) can never pass with the stand-in vault, so the RTTI system is the test, and the post-register queue that would never run is replaced by a log line.
- No legacy-folder cleanup (`r6/scripts/mod_settings`, `archive/pc/mod/ModSettings.*`): no Mac install ever used them.
- The scripts come from `<plugin folder>/Scripts/` (`packed.reds`, `module.reds`), compiled into the script cache by `cp2077-codeware/tools/build-cache.py`.
- `user.ini` is `<plugin folder>/user.ini`, the folder found with `dladdr`.
- The fundamental types for `RedTypeFromString` are looked up by name (`Bool`, `Int32`, ...); the Windows fixed hashes (`RTTITYPE(hash)`) have no Mac address.
- `AddVariable` is exported from the dylib, the runtime API other mods use. A Mac plugin that wants it (LTBF) calls `dlsym(RTLD_DEFAULT, "AddVariable")`;
  `__declspec(dllimport)` means nothing on the Mac. `modsettings_mac_pending` is a diagnostic export for the offline smoke test.
- fmt and spdlog are not linked: Mod Settings does not use them (`otool -L` shows libc++ and libSystem only), so none are staged.
- No `MessageBox`, no retry loop: one `Attach` attempt per hook with one log line (`ATTACHED`, `CHAINED`, `NOT ACTIVE`, `REFUSED`, `FAILED`).
  `NOT ACTIVE` means RED4ext-macos accepted the call but the offset is not on its allowlist (`hook-offsets.txt`).
- `MODSETTINGS_MAC_DIAG=1` in the environment turns on the vault and vslot diagnostics.
- The vault ID stand-ins (`tweakxl_rtti_async_id` 0x7FFB0000, `tweakxl_serializable_counter` base 0x7C00000000000000) are Mod Settings' own bands, distinct from TweakXL, Codeware, ArchiveXL and LTBF: `ltbf/VAULT-IDS.md`.
