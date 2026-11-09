# History

Decisions and findings, newest first. One entry per finding: what was decided, why, the numbers
that showed it, what was tried and dropped, and the commits. Code comments point here instead of
carrying dates and measurements.

## 2026-11-09 Dependencies with a value, saving, Defaults, Back in the pause menu

- `ModSettings.dependency "[Class.]prop=value"` never matched (#21): the value kept its `=` and the property name kept
  `=value`. `ReadDependency` returns the dependency by value now, so it no longer leaks a `calloc` per field.
- Back saved twice (`OnBack` called `AcceptChanges` after `CheckSettings` had), and every save backs up `user.ini`
  first, so `user.ini.bak` always held the file just written. `AcceptChanges` now writes only when a variable has an
  unaccepted change, and a save accepts (and so notifies listeners about) only the variables that changed.
- Defaults on one mod reset the global `changesRequested`, hiding Apply and Reset for other mods' pending changes;
  it is recomputed from every variable (`ModSettings::HasChanges`).
- Rows that depend on a setting show or hide when it changes, not only after Apply: a change queues
  `ModStngsRowsChangedEvent` and the page is rebuilt a frame later if its rows differ. The page returns early when no
  mod has settings, instead of reading `m_data[0]` of an empty array.
- Back in Mod Settings opened from the pause menu never saved: the game's `MenuScenario_PauseMenu.GoBack` hands
  `OnBack` to `settings_main` only and switches straight to `pause_menu` for any other page, so unapplied changes
  stayed pending and were lost at exit. `MenuScenario_PauseMenu.OnBack` is wrapped to hand it to
  `mod_settings_main`, which saves and asks to close (`OnCloseModSettingsScreen`, now handled there too). The main
  menu's `MenuScenario_ModSettings` already did this. Older than these changes: the smoke test's new Back step
  found it.
- The dependency parsing is in `DependencyString.hpp`, plain `std::string`, with the repo's first offline tests
  (`tests/`, `ctest --test-dir build`, run by `build.yaml`).
- In game, the smoke test passed 11 of 11, twice: both dependency forms, Defaults on a second mod, a save with nothing
  changed leaving `user.ini` and `user.ini.bak` byte for byte, one accepted notification for one changed variable,
  and on the open page "Shown at 7" listed after requesting `testNumber=7` without Apply, then Esc saving once
  (`user.ini.bak` the file from before) and returning to the pause menu. The menu step's earlier misses were the
  pause menu not opening right after the load (`../cp2077_tools/docs/mod_tests_plan.md`). Not seen in the game: the
  empty-page guard (it needs a run with no mod registering settings) and Back from the main menu's page.

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
