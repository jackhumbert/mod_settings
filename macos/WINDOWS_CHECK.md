# Windows-view check: mod_settings

Repo: `ltbf/mod_settings`
Branch: `refs/heads/macos`
Base: tag `v0.2.21`

Method: `bash wincheck.sh <repo> v0.2.21`, then for each listed file

`diff -u <(git show v0.2.21:<f> | unifdef -U__APPLE__ -t) <(git show refs/heads/macos:<f> | unifdef -U__APPLE__ -t)`

and the full `git diff v0.2.21 refs/heads/macos -- <f>`. Line numbers below are the `macos` branch.

Other files `git diff --name-only v0.2.21 refs/heads/macos` lists, which this script did **not** flag (Windows view matches the tag): `src/red4ext/ActionNameEx.hpp`, `Hooks/ApplyOverrides.hpp`, `Hooks/ReleaseScriptData.cpp`, `ModSettings.hpp`, `Variable.cpp`, `stdafx.hpp`.

No `RED4EXT_ASSERT_OFFSET` line and no struct field appear in any Windows-view diff for this repo.

## Table

| file | hunk (short) | class | reason | suggested fix (CHANGE) |
| --- | --- | --- | --- | --- |
| `src/red4ext/Hooks/ApplyOverrides.cpp` | blank lines before `ApplyOverrides_Original` and after the hook body | NO-OP | Whitespace only. The hook function body is unchanged in the Windows view. | |
| `src/red4ext/Main.cpp` | blank line before `TypeInfoRegistrar::RegisterDiscovered` | NO-OP | Whitespace only. | |
| `src/red4ext/ModSettings.cpp` | blank line before `ModSettings::AddOverrides` | NO-OP | Whitespace only. | |
| `src/red4ext/Utils.cpp` | blank line before `Utils::ToWString` | NO-OP | Whitespace only. | |
| `src/red4ext/Utils.hpp` | `Utils::FromString` plus `MS_FUNDAMENTAL_TYPE`, used by every `RedTypeFromString` specialization except `CName` (`Utils.hpp:17-87`) | EQUIVALENT | Windows `FromString` (`Utils.hpp:23-27`, the `#else`) builds the same `RawBuffer` the tag built inline: `buffer.data = (void*)str.c_str()` and `buffer.size = str.length`, then `type->FromString(instance, buffer)`. Callers still ignore the bool. `MS_FUNDAMENTAL_TYPE(hash, name)` is `#define MS_FUNDAMENTAL_TYPE(hash, name) RTTITYPE(hash)` (`Utils.hpp:37`). `RTTITYPE` is `#define RTTITYPE(hash) RED4ext::UniversalRelocFundamental<RED4ext::CBaseRTTIType>(hash)` (`deps/red4ext.sdk/include/RED4ext/RTTITypes.hpp:18`). The hashes in the specializations are the tag hashes: Bool `3443526637`, Int8 `3415542724`, Int16 `3592162291`, Int32 `3590851569`, Int64 `3594652662`, Uint8 `3648392249`, Uint16 `3832679528`, Uint32 `3831368806`, Uint64 `3835169899`, Float `3665562711`, Double `3892120764`. The macOS arm of `FromString` passes the `CString` directly (`Utils.hpp:21-22`) and the macOS `MS_FUNDAMENTAL_TYPE` calls `GetType(CName(name))` (`Utils.hpp:35`); unifdef removes both. `RedTypeFromString<CName>` is unchanged (`Utils.hpp:89-91`). `buffer.size` is the raw `length` field, which is what the tag specializations assigned. `CString::Length()` (`deps/red4ext.sdk/include/RED4ext/CString-inl.hpp:141-143`, `length & 0x3FFFFFFF`) is a different function and is not what these sites used on the tag or use now. | |
| `src/red4ext/ScriptDefinitions/ScriptProperty.hpp` | `#include "Utils.hpp"` and five `RawBuffer` blocks replaced by `Utils::FromString` (`ScriptProperty.hpp:80`, `:91`, `:145`, `:152`, `:170`) | EQUIVALENT | Each tag block was `buffer.data = (void*)str->c_str(); buffer.size = str->length;` then `GetType("Uint32"|"Int32"|"Float")->FromString(...)`. `Utils::FromString` on Windows does those two assignments from the `const CString&` bound to `*str` and calls the same `FromString` (`Utils.hpp:23-27`). `GetType("Uint32")` / `GetType("Int32")` / `GetType("Float")` are still the type argument. The return bool is still ignored. `ScriptProperty::FromString` itself is unchanged on Windows: its `#else` still uses `str.Length()` and `this->GetType()->FromString` (`ScriptProperty.hpp:48-52`) and does not appear in the unifdef diff. | |

## Summary

| class | count |
| --- | --- |
| NO-OP | 4 |
| EQUIVALENT | 2 |
| CHANGE | 0 |
| total rows | 6 |

No Windows-visible hunk in mod_settings changes behavior or layout. The shared edits are whitespace, or a wrapper whose Windows body is the tag's `RawBuffer` plus `RTTITYPE(hash)` sequence.


Later macOS-only changes (the Bool default parsing in `ScriptDefinitions/ScriptProperty.hpp` and the value-initialised defaults in
`RuntimeVariable.hpp`) are under `#ifdef __APPLE__`; their Windows view is unchanged.

Re-run the check:

```
for f in $(git diff --name-only v0.2.21 HEAD -- . ':!macos'); do
  diff -q <(git show v0.2.21:$f | unifdef -U__APPLE__ -t) <(unifdef -U__APPLE__ -t $f) >/dev/null || echo "$f"
done
```
