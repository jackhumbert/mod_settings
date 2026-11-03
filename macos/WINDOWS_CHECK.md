# Windows-view check: mod_settings

Repo: `ltbf/mod_settings`
Branch: the macOS branch (`HEAD`), rebased onto `main` on 2026-10-14, then merged with `main` on 2026-11-03
Base: `main` (`26d6715`, 2026-10-28: upstream's game 2.5 fixes, including the 2.5 script definition layouts `CScriptedDataEnvironment`,
`CScriptedDataClass`, `CScriptedDataProperty` and the 2.5 `UserMapping::Mapping`). The first version of this check was made against
`v0.2.21`, the second against `main` `e904815` after the rebase. The table below is the result of the run against `26d6715` after the
merge.

Method: for every shared file the branch changes (outside `macos/` and `deps/`), compare the Windows view with `main`, then for each
listed file

`diff -u <(git show main:<f> | unifdef -U__APPLE__ -t) <(git show HEAD:<f> | unifdef -U__APPLE__ -t)`

and the full `git diff main HEAD -- <f>`. Line numbers below are the branch (`HEAD`).

Files compared: the 15 files `git diff --name-only main HEAD -- . ':!macos' ':!deps'` lists. Windows view identical to `main` (no row
below): `src/red4ext/ActionNameEx.hpp`, `ModSettings.hpp`, `RuntimeVariable.hpp`, `ScriptData.hpp`,
`ScriptDefinitions/ScriptDefinitions.hpp`, `Variable.cpp`, `stdafx.hpp`. The other 8 differ and have rows below; one of them
(`Hooks/ReleaseScriptData.cpp`) differs only by a final newline.

The submodule pointers (`deps/red4ext.sdk`, `deps/cpcmake` and the others) are the ones `main` has. The Mac build does not use
`deps/red4ext.sdk`, `deps/cpcmake` or `deps/archive_xl` (it builds against the macOS RED4ext SDK; `macos/CMakeLists.txt`), only
`deps/red_lib/vendor` (nameof).

No `RED4EXT_ASSERT_OFFSET` line and no struct field appear in any Windows-view diff for this repo.

## Table

| file | hunk (short) | class | reason | suggested fix (CHANGE) |
| --- | --- | --- | --- | --- |
| `src/red4ext/Hooks/ApplyOverrides.cpp` | blank lines before `ApplyOverrides_Original` and after the hook body | NO-OP | Whitespace only. The hook function body is unchanged in the Windows view. | |
| `src/red4ext/Hooks/ApplyOverrides.hpp` | comments on `keyMap` and `rawMappings` (`// 70 (2.5 Mac: 78)`, `// 98 (2.5 Mac: A0)`); file ends with a newline | NO-OP | Comment text and the final newline only. The macOS `CalcHash`, the `UserMapping` member the Mac 2.5 build has at +0x70, the Mac `static_assert`s on `Mapping` and the `ApplyOverridesMac` declarations are in `#ifdef __APPLE__` arms; the Windows `#else` keeps the `main` `UniversalRelocFunc` `CalcHash` (hash `2583235069`). `Mapping` (0x40, `overridableUI` at 0x34) is `main`'s 2.5 layout for both platforms. | |
| `src/red4ext/Hooks/ReleaseScriptData.cpp` | file ends with a newline | NO-OP | The macOS hook (shared with Codeware's chain) is in the `#ifdef __APPLE__` arm; the Windows `#else` keeps the `main` `REGISTER_HOOK_HASH` body with `CScriptedDataEnvironment`. The only difference is the final newline that the `#endif` line adds. | |
| `src/red4ext/Main.cpp` | blank line before `TypeInfoRegistrar::RegisterDiscovered` | NO-OP | Whitespace only. `main`'s `RemoveLegacy` cleanup, the checked `scripts->Add` / `RegisterArchive` calls and the "Attaching hooks" / "Mod Settings loaded" log lines are the `#ifndef __APPLE__` / `#else` arms, unchanged. | |
| `src/red4ext/ModSettings.cpp` | blank line before `ModSettings::AddOverrides` | NO-OP | Whitespace only. The macOS `fopen` is in `#ifdef __APPLE__` (`ModSettings.cpp:555-560`) with the `main` `_wfopen` line in the `#else` (`:559`). The Mac early return in `AddOverrides` (`:382-386`) and the Mac `else if` for `ApplyOverridesMac` in `AcceptChanges` (`:583-586`) are removed by unifdef; `AcceptChanges` keeps `main`'s `if` / `else` warning. | |
| `src/red4ext/Utils.cpp` | blank line before `Utils::ToWString` | NO-OP | Whitespace only. | |
| `src/red4ext/Utils.hpp` | `Utils::FromString` plus `MS_FUNDAMENTAL_TYPE`, used by every `RedTypeFromString` specialization except `CName` (`Utils.hpp:17-102`) | EQUIVALENT | Windows `FromString` (`Utils.hpp:39-42`, the `#else`) builds the same `RawBuffer` `main` built inline: `buffer.data = (void*)str.c_str()` and `buffer.size = str.length`, then `type->FromString(instance, buffer)`. Callers still ignore the bool. `MS_FUNDAMENTAL_TYPE(hash, name)` is `#define MS_FUNDAMENTAL_TYPE(hash, name) RTTITYPE(hash)` (`Utils.hpp:52`). `RTTITYPE` is `#define RTTITYPE(hash) RED4ext::UniversalRelocFundamental<RED4ext::CBaseRTTIType>(hash)` (`deps/red4ext.sdk/include/RED4ext/RTTITypes.hpp:18`). The hashes in the specializations are the `main` hashes: Bool `3443526637`, Int8 `3415542724`, Int16 `3592162291`, Int32 `3590851569`, Int64 `3594652662`, Uint8 `3648392249`, Uint16 `3832679528`, Uint32 `3831368806`, Uint64 `3835169899`, Float `3665562711`, Double `3892120764`. The macOS arm of `FromString` passes the text as the game's (pointer, length) pair, `MacTextView` (`Utils.hpp:18-26`, `:31-37`), and the macOS `MS_FUNDAMENTAL_TYPE` calls `GetType(CName(name))` (`Utils.hpp:50`); unifdef removes all of them. `RedTypeFromString<CName>` is unchanged (`Utils.hpp:104-106`). `buffer.size` is the raw `length` field, which is what the `main` specializations assigned. `CString::Length()` (`deps/red4ext.sdk/include/RED4ext/CString-inl.hpp`, `length & 0x3FFFFFFF`) is a different function and is not what these sites used on `main` or use now. | |
| `src/red4ext/ScriptDefinitions/ScriptProperty.hpp` | `#include "Utils.hpp"` and five `RawBuffer` blocks replaced by `Utils::FromString` (`ScriptProperty.hpp:105`, `:116`, `:170`, `:177`, `:195`) | EQUIVALENT | Each `main` block was `buffer.data = (void*)str->c_str(); buffer.size = str->length;` then `GetType("Uint32"|"Int32"|"Float")->FromString(...)`, with `str` from `this->attributes.Get(name)`. `Utils::FromString` on Windows does those two assignments from the `const CString&` bound to `*str` and calls the same `FromString` (`Utils.hpp:39-42`). `GetType("Uint32")` / `GetType("Int32")` / `GetType("Float")` are still the type argument, and the two `&value` and three `pointer` destinations are unchanged. The return bool is still ignored. `CScriptedDataProperty::FromString` itself is unchanged on Windows: its `#else` still uses `str.Length()` and `this->GetType()->FromString` (`ScriptProperty.hpp:73-78`) and does not appear in the unifdef diff. | |

## Summary

| class | count |
| --- | --- |
| NO-OP | 6 |
| EQUIVALENT | 2 |
| CHANGE | 0 |
| total rows | 8 |

No Windows-visible hunk in mod_settings changes behavior or layout. The shared edits are whitespace, comments, a final newline, or a
wrapper whose Windows body is the `main` `RawBuffer` plus `RTTITYPE(hash)` sequence.


Later macOS-only changes (the Bool default parsing in `ScriptDefinitions/ScriptProperty.hpp`, the value-initialised defaults in
`RuntimeVariable.hpp`, the (pointer, length) text argument of `FromString` in `Utils.hpp`, the `fopen` in `ReadFromFile`, and the
`static_assert`s that check the 2.5 offsets on the Mac, where the SDK's `RED4EXT_ASSERT_OFFSET` is empty under clang) are under
`#ifdef __APPLE__`; their Windows view is unchanged. The Mac build's `GetCurrentThreadId` (used by `main`'s `AddOverrides` log line) is
a shim in `macos/MacCompat.hpp`, which only the Mac build includes.

Re-run the check:

```
for f in $(git diff --name-only main HEAD -- . ':!macos' ':!deps'); do
  diff -q <(git show main:$f 2>/dev/null | unifdef -U__APPLE__ -t) <(git show HEAD:$f | unifdef -U__APPLE__ -t) >/dev/null || echo "$f"
done
```
