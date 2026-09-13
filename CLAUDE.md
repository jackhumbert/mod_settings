# Mod Settings

Cyberpunk 2077 mod: adds a redscript-configurable "Mod Settings" page to the main/pause menu that other mods add sections to. RED4ext C++ plugin + redscript + ArchiveXL archive, all shipped under `red4ext/plugins/mod_settings/`.

Sibling repos with the same build/release shape: `../flight_control` (Let There Be Flight, the most documented one; read its CLAUDE.md for the full picture), `../in_world_navigation`, `../input_loader`.

## Layout

- `src/red4ext/` - plugin. `Main.cpp` = entry. `Hooks/` = 2 hooked game functions plus 1 direct call, all by RED4ext address hash (`REGISTER_HOOK_HASH` / `UniversalRelocFunc`). `Scripting/`, `ScriptDefinitions/` = the script-facing API.
- `src/redscript/mod_settings/` - the UI and API redscript; packed into `packed.reds` + `module.reds`. Other mods compile against this folder (LTBF and In-World Navigation vendor this repo as a submodule for that).
- `src/wolvenkit/` - Wolvenkit project; the built `ModSettings.archive` under `packed/` is committed and only copied by CMake.
- `deps/` - submodules: `red4ext.sdk` (jackhumbert fork, branch `new-types`), `cpcmake` (jackhumbert/cyberpunk_cmake, the CMake framework), `red_lib` (fork, branch `jack`), `archive_xl` (headers only), `spdlog`, `detours`.
- `game_dir/` build output in game-folder layout, `game_dir_debug/` PDBs; both zipped for release. `requirements.md` is regenerated at configure time from the Find modules in cpcmake.

## Build

MSVC 2022 + Ninja + CMake 3.24+ (VS dev shell). Same commands as LTBF:

```
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -G Ninja
cmake --build build
cmake --install build        # copies game_dir(+debug) into CYBERPUNK_2077_GAME_DIR
```

Game version for the release name comes from the installed `Cyberpunk2077.exe`, or in CI from the last entry of `deps/red4ext.sdk/cmake/GameVersions.json`.

## Updating for a game patch

1. `python tools/check_hashes.py` - checks the 3 address hashes against the game's shipped `bin/x64/cyberpunk2077_addresses.json`. Present = resolves at runtime.
2. Bump `deps/red4ext.sdk` to the fork commit that knows the patch (must exist in `GameVersions.json` and `Api/v0/Runtime.hpp`), and `deps/cpcmake` for the requirement floors. Do the SDK work in the `flight_control` checkout and reuse the commit here.
3. Build, install, launch, confirm `red4ext/logs/mod_settings-*.log` shows the hooks attaching and settings loading.
4. Tag.

State on 2026-09-13: v0.2.21 (built for 2.30) loads and works on game 2.31 unchanged; pins bumped to the 2.31 SDK so the next tag reads "for 2.31".

## Releases

Push a `vX.Y.Z` tag. `.github/workflows/release.yaml` builds, runs git-cliff over conventional commits (`feat:`/`fix:` show up; `chore:`/`ci:` are skipped), publishes the GitHub release, then uploads the zip to Nexus mod 4885 (file id 2608471) with the changelog through `Nexus-Mods/upload-action`. The Nexus step needs the repo secret `NEXUSMODS_API_KEY` and resolves the v3 mod id from the site id first. Checkout is full-depth on purpose; a shallow clone gives git-cliff no history and an empty changelog.

Push submodule commits to their own repos before tagging; CI clones them over HTTPS.

## Conventions

- No em-dashes anywhere.
- Don't commit `build/`, `game_dir*/`, `compile_commands.json`, or Wolvenkit `.projectFiles` churn.
