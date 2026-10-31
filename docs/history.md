# History

Decisions and findings, newest first. One entry per finding: what was decided, why, the numbers
that showed it, what was tried and dropped, and the commits. Code comments point here instead of
carrying dates and measurements.

## 2026-10-31 Hooks renamed to the game's own names

Decision: every hook and direct call whose address hash has a SHA-confirmed name in
`IDA_Cyberpunk_2077` is named `<Class>_<Method>` after it, with the demangled signature above it,
as LTBF did the same day. Why: `ReleaseScriptData` (3993832650) is really
`CScriptedDataEnvironment::~CScriptedDataEnvironment`, so it is now `CScriptedDataEnvironment_dtor`
and its file follows. `ApplyOverrides` is `Manager_ApplyOverrides` (`game::input::Manager`), and the
direct call `ClearRuntimeDataOnRawMappings` is `ClearRuntimeDataOnRawMappings_Expensive`. `SkipKey`
(`ink::SkipKey`) and `CalcHash` (`red::String::CalcHash`) were already right. The commented-out hooks
got their names too; `ListenForInputCallback` (3729004759) is the one hash not named yet.
`Hooks/ApplyOverrides.*` keep their names: they hold the input-override code, not one hook.

## 2026-09-13 Game 2.31

v0.2.21 (built for 2.30) loads and works on game 2.31 unchanged; pins bumped to the 2.31 SDK so the
next tag reads "for 2.31".
