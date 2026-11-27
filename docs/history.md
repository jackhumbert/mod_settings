# History

Decisions and findings, newest first. One entry per finding: what was decided, why, the numbers
that showed it, what was tried and dropped, and the commits. Code comments point here instead of
carrying dates and measurements.

## 2026-11-27 Menu and listener costs with many settings

- Profiled with 2,400 settings over 30 classes (`../cp2077_tools/mods/mod_settings/stress_gen.py`, `perf.py`; the
  longest frame from the bridge's `longest_frame_ms`). Mod Settings' C++ was 0.02% of the menu's time: the cost is
  the page's redscript and creating and destroying row widgets, about 0.3 ms a row. Old menu code against new, the
  plugin the same:

  | | before | after |
  |---|---|---|
  | open a 240-setting page | 104 to 122 ms | 35 ms |
  | search matching all 2,400 | 1,520 ms | 46 ms |
  | narrow it to 11 results | 1,470 ms | 35 ms |
  | clear the search | 1,260 ms | 17 ms |
  | a value change while results show | 28 ms | 8.5 ms |
  | a value change on a page | 1.6 ms | 1.4 ms |

- Rows spawn 30 a frame (`ModStngsSpawnRowsEvent`, a generation number so a newer page cancels the rest); the
  previous page's rows are hidden at once and removed 60 a frame, as removing 240 at once took about 40 ms.
- The search shows the first 100 results, under a `UI-ModSettings-MoreResults` line ("Showing 100 of N matching
  settings") when more match. Spawning and then clearing a row for every match was most of the 1.5 s.
- A value change rebuilt nothing unless rows showed or hid, but finding out built a string of every listed row's
  path twice and, while searching, read every setting's localized text twice. Now it compares group and name
  CNames, and keeps each setting's earlier search match (`ComputeSearchMatches`: `GetVars` leaves out hidden
  dependents, so the previous results are walked alongside, up to 64 ahead).
- `ModClass::listeners` (the weak handles `RegisterListenerToClass` gives each settings class) was never pruned: a mod
  that drops its objects without unregistering left them for good, and every accepted change still went through
  them. Registering now drops expired entries and an earlier copy of the same instance. Accepting with 20,000 live
  listeners takes 16 ms against 10 ms with none; after dropping them, 9 ms.
- The smoke test passed 16 of 16 (dependent rows still show and hide); screenshots showed the capped search with its
  line on top and a 240-setting page filling in.

## 2026-11-25 Dependencies on a class declared in a module

- `ModSettings.dependency "Module.Class.prop"` never resolved (#37): the parser split class from property at the first
  period, giving class `Module`, and the row always showed. A class in a module is registered under its full name
  (the game's log has `Loaded ClaudeModSettingsModule.ClaudeModuleSettings.moduleSwitch`), and property names hold no
  period, so the split is at the last one. The value is cut off at `=` before this, so `scale=1.5` is unaffected.
- Offline test `module_qualified_class`. The smoke test got a test mod with a class in a module and a row depending on
  it from outside (`../cp2077_tools/mods/mod_settings/mod_settings_module.reds`). In the game (2026-11-26, after two
  launches had failed while another job held the GPU) the smoke test passed 16 of 16: the row hidden with the switch
  off, listed with it on, hidden again. A launch with the v0.2.35 release's own files loaded normally too, checked
  after a report that it closed the game at the loading screen.

## 2026-11-25 Search across every mod's settings

- A search bar above the mod list filters the page to the settings, from every mod, whose name, description, mod or
  category hold all the words typed (case ignored, two characters minimum: one letter matches nearly every setting and
  spawns a row for each). Results are the live rows under a `Mod · Category` header each (the mod alone for its
  uncategorised settings), so Apply, Reset, Back and dependent rows work as on a mod's page. The `·` renders in the
  page's font (raj). Picking a mod clears the search; Defaults and its hint are hidden while searching.
- The bar is the 2.5 mod.io browser's (`base\gameplay\gui\fullscreen\main_menu\modio_browser_menu.inkwidget`,
  `searchBarCanvas`), copied into `mod_settings_main.inkwidget` as the first child of `mod_settings` (renamed
  `search_bar`, `search_input`, `search_placeholder`; its `ModMenuSearchBarController` and list-item controller
  dropped, as they need the browser's game controller). The copy was made on WolvenKit 10 JSON: the file's handles
  are numbered across both copies of the tree (`package` and the cooked `packageData`), so the subtree went into both
  and the whole file was renumbered.
- The controller finds the bar's widgets by path. Script fields added to the widget's `gameController` did not bind
  (the cooked copy held the names once, not in both trees), so the page's own fields stay those of the game's
  settings controller. The placeholder is the game's `UI-Modio-FiltersSorting-Search`; the no-results line is our
  `UI-ModSettings-NoResults`.
- Input: X on a pad (and on the keyboard) focuses the bar through `controller_settings`, the settings menu's spare
  button, which this page hides (`focus_search_bar` exists only in the mod browser's input context). While the bar
  has focus the page's shortcuts are ignored, the rows are not interactive (a hovered row takes A/D as its own), a
  click elsewhere leaves the bar, and Back leaves the bar before it closes the page.
- While results show, the mod list greys out: each entry goes to its `Default` state (no `Selected` highlight on the
  open mod) and its label's `tintColor` and `opacity` bindings are swapped for `MainColors.Grey` at 0.4, then put back
  (`MenuLabel.MainColor`, `MenuLabel.MainOpacity`). The label's style-bound opacity wins over opacity set on the entry,
  `mods` or `mod_flex` (none showed), and the HDR red faded alone only from 255 to 177 in a screenshot.
- The smoke test puts text in through a helper calling `OnSearchTextChanged`: the bridge's keys are game input, which
  a text field does not take as typing. Typing on a real keyboard was checked by hand.

## 2026-11-25 The menu entries are added at load, not shipped as copies of the game's menu files

- `ModSettings.archive` carried full copies of `menu.inkmenu` and `pregame_menu.inkmenu` with the page's entry (and
  the pregame `MenuScenario_ModSettings`) added. Copies go stale with every patch that adds a menu: the 2.3 copies on
  2.5 lacked `glamdoc` and five others, and the Glamdoc screen came up empty (fixed by rebuilding them in 0.2.27, and
  again offered by a third-party archive and #34). Two mods shipping copies of one file also leave only one working.
- Now `Hooks/ResourceSerializer_OnDependenciesReady.cpp` adds them when the game loads either file: a copy of the
  game's `settings_main` entry with our name and widget, and the scenario name. It skips a file that already has the
  entry (an old archive). The hook is the one ArchiveXL's resource patching uses (hash 3892979844, finished objects
  at 0x230); ArchiveXL's own patching does not cover `inkMenuResource`, and Codeware's `Resource/PostLoad` would have
  added a requirement. The loader has already fetched the entries' widgets at this point, so ours is loaded here.
- Checked in game with an archive holding only the widget and localization: both files came through the hook at
  startup with vanilla's 35 and 60 entries and got ours; the smoke test passed, the page opening from the pause menu
  and, in a new step, from the main menu (the added scenario).

## 2026-11-20 Lists from C++ (enum variables), ApiVersion

- Let There Be Flight's Land at Destination became a three-way list registered from C++, and `AddVariable` had no
  enum path (the case was commented out). `AddVariable` now takes an enum type's name as `type`: a list of its values
  in order, each shown as the localization key `<displayName>-<value name>`, the value and default an index. No change
  to the `Variable` struct, so plugins built against the old header still work.
- Saved text is a value's name (as `Write` writes it, through the enum type) or an index. A Bool saved under the same
  name carries over as the list's first or second value: `Write` saves Bools as `false` / `true` (Jack's file has
  `landAtDestination = true`), so those are read as 0 and 1. LTBF relies on this.
- `ApiVersion()` (exported, 1) tells a plugin lists are there; older releases lack the export. A type it does not know
  is logged with the variable's name (the format string was `{}` in a printf-style call).
- Checked in game through LTBF (its `wheel_landing.py`, this build's dll put in for the run): the list registered,
  showed and set by index through `ModConfigVarEnum`, and saved as the value's name.

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
