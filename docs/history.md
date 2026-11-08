# History

Decisions and findings, newest first. One entry per finding: what was decided, why, the numbers
that showed it, what was tried and dropped, and the commits. Code comments point here instead of
carrying dates and measurements.

## 2026-11-08 Categories and rows in the order a mod gives

- Rows ignored `ModSettings.order`: the three-argument `ReadProperty` parsed the attribute with the field's own type
  (a Bool or a Float field), so a row's order came out wrong. It is read as a `Uint32` now
  (`ScriptDefinitions/ScriptProperty.hpp`).
- A category made by `AddVariable` (C++) had order 0 and listed before every script category, even one a script
  class gave `ModSettings.category.order`. A category's order is now the lowest any class gives it, from the first
  field in each class that gives one; a category no class orders lists after the ordered ones, in the order found
  (`GetCategories`, `ModCategory::kUnordered`). No change to the `Variable` ABI.
- In game with Let There Be Flight's re-organised settings (`../cp2077_tools/mods/let_there_be_flight/
  settings_menu_check.py`, ManualSave-191): its eight categories and every row in the order its fields give, which
  0.2.28 did not. The smoke test passed 5 of 6 twice; the pause menu step did not find the menu list, the timing
  failure `../cp2077_tools/docs/mod_tests_plan.md` notes, before any Mod Settings page opens.

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
