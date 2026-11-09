# Mod Settings

Cyberpunk 2077 mod: adds a redscript-configurable "Mod Settings" page to the main/pause menu that other mods add sections to. RED4ext C++ plugin + redscript + ArchiveXL archive, all shipped under `red4ext/plugins/mod_settings/`.

The rules for every mod (languages, native design, layout, build, release, comments, docs, tests) are in `../cp2077_tools/docs/mod_standard.md`; this file has only what is particular to Mod Settings. Sibling repos: `../let_there_be_flight`, `../in_world_navigation`, `../input_loader`.

## Layout

- `src/red4ext/` - plugin. `Main.cpp` = entry. `Hooks/` = 3 hooked game functions plus 3 direct calls, all by RED4ext address hash (`REGISTER_HOOK_HASH` / `UniversalRelocFunc`), named `<Class>_<Method>` after the game function. `docs/history.md` = decisions and findings. `Scripting/`, `ScriptDefinitions/` = the script-facing API.
- `src/redscript/mod_settings/` - the UI and API redscript; packed into `packed.reds` + `module.reds`. Other mods compile against this folder (LTBF and In-World Navigation vendor this repo as a submodule for that).
- `src/wolvenkit/` - Wolvenkit project; the built `ModSettings.archive` under `packed/` is committed and only copied by CMake.
- `deps/` - submodules: `red4ext.sdk` (jackhumbert fork, branch `new-types`), `cpcmake` (jackhumbert/cyberpunk_cmake, the CMake framework), `red_lib` (fork, branch `jack`), `archive_xl` (headers only), `spdlog`, `detours`.
- `game_dir/` build output in game-folder layout, `game_dir_debug/` PDBs; both zipped for release. `requirements.md` is regenerated at configure time from the Find modules in cpcmake.

## Build

Standard (section 6): `cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja`, then `python ../cp2077_tools/game.py build "<title>"`; install only through `game.py run --install`.

Game version for the release name comes from the installed `Cyberpunk2077.exe`, or in CI from the last entry of `deps/red4ext.sdk/cmake/GameVersions.json`.

## Updating for a game patch

1. `check_hashes.py` (shared game tools, see `CLAUDE.local.md`) - checks the 3 address hashes against the game's shipped `bin/x64/cyberpunk2077_addresses.json`. Present = resolves at runtime.
2. Bump `deps/red4ext.sdk` to the fork commit that knows the patch (must exist in `GameVersions.json` and `Api/v0/Runtime.hpp`), and `deps/cpcmake` for the requirement floors (standard, section 5).
3. Build, install, launch, confirm `red4ext/logs/mod_settings-*.log` shows the hooks attaching and settings loading.
   - Install, launch and test only through `../cp2077_tools`: the one way to test, see `E:\src\CLAUDE.md` and `../cp2077_tools/README.md`. Other sessions share the game install and Jack plays on it; never start, kill or install into the game around it. The smoke test:

     ```
     python ../cp2077_tools/game.py run "<title>" --purpose "mod_settings smoke" --install --launch --scripts ../cp2077_tools/mods/mod_settings --protect red4ext/plugins/mod_settings/user.ini -- python ../cp2077_tools/mods/mod_settings/smoke.py --out <dir>
     ```

     It checks that the log shows the hooks attached and the settings loaded, and that a test settings class
     is listed with its defaults. It checks that a change accepted through the API reaches the variables and
     a `RegisterListenerToClass` instance, dependencies, Defaults and saving, and that the pause menu's Mod
     Settings page shows the test mod, rebuilds its dependent rows and saves on Back (with screenshots).
     `--protect` puts Jack's `user.ini` back as it was. Leave out `--install` to test what is installed.
   - Offline unit tests (`tests/`): `ctest --test-dir build` after a build.
   - RED4ext mods load however the game is started; the 2.5 menu's "Mods" entry and its "sign into your CD PROJEKT RED account" popup belong to CDPR's mod.io browser, not to us.
   - A plugin not built for the current patch can crash startup right after RED4ext logs "Loading plugin from ..." for it. Rule it out with `ignored = ["<plugin folder>"]` in `red4ext/config.ini`.
4. Tag.

Past patch-day states are in `docs/history.md`.

## Releases

The standard release flow (section 5). Nexus mod 4885, file id 2608471.
