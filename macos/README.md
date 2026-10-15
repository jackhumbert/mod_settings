# Mod Settings for macOS (Apple Silicon)

A port of [jackhumbert/mod_settings](https://github.com/jackhumbert/mod_settings) `main` (e904815, which is v0.2.21 plus 8 commits) to
the native macOS build of Cyberpunk 2077 (2.3.1, Steam, Apple Silicon). Port by codeman101 (skyman3215). MIT, like upstream.

Everything macOS-specific is in this folder and in `#ifdef __APPLE__` blocks in `src/red4ext`. The Windows build is meant to be
unchanged: `WINDOWS_CHECK.md` lists the shared files whose Windows view (`unifdef -U__APPLE__`) differs from `main` and why each
difference is equivalent. What is not done yet: `NOT_YET.md`. Layout evidence for what the plugin reads from game memory: `LAYOUT.md`.

## Status

Tested on 2.3.1 with Let There Be Flight for macOS, in automated in-world runs:

- Settings > Mods lists Let There Be Flight with its 12 categories and 66 settings.
- A changed value (`engineVolume` 1.0 to 0.75) reaches the mod's own object at once, is written to `user.ini`, comes back after a restart,
  and can be set back.
- Bool settings keep their script defaults (fixed on macOS: every `true` default used to come out `false`, see `NOT_YET.md` 0b).
- Rebased onto `main` on 2026-10-14. The runs above were made on the v0.2.21 base. The rebased build (`main` e904815 plus the Mac commits)
  loads, lists the settings and reads the existing `user.ini` (the menu was opened by hand on 2026-10-14).

Key-binding overrides (`ApplyOverrides`) are wired but have not been exercised in game; see `NOT_YET.md`.

## What you need to run it

- Cyberpunk 2077 2.3.1, the native macOS build (Steam), on Apple Silicon.
- A RED4ext loader for macOS that calls `modsettings_registerNatives` after `tweakxl_registerNatives`, `archivexl_registerNatives` and
  `codeware_registerNatives`, at script initialisation. The port was developed on a loader setup that does this from a Frida gadget
  script. jackmaxwil's public [RED4ext-macos](https://github.com/jackmaxwil/RED4ext-macos) uses the game's post-register callback
  instead; support for it is planned.
- The macOS ports of ArchiveXL and [Codeware](https://github.com/codeman101/cp2077-codeware/tree/macos) (the `ReleaseScriptData`
  hook is chained through Codeware's).

## Build

```
cmake -S macos -B macos/build -DCMAKE_BUILD_TYPE=Release
cmake --build macos/build -j8
```

Needs the Xcode command line tools, CMake 3.21+, Homebrew `fmt` and `spdlog`, and the macOS RED4ext SDK and Codeware checkouts next to this
repository (see the header of `CMakeLists.txt`; override with `-DRED4EXT_SDK_DIR=` and the other cache paths there).
`-DMODSETTINGS_MAC_APPLYOVERRIDES=OFF` compiles the key-binding override hook out (then remove its offset from `hook-offsets.txt`).

## Install layout

`red4ext/plugins/mod_settings/`: `mod_settings.dylib`, `license.md`, `Scripts/` (`packed.reds`, `module.reds`, compiled into the script
cache by the loader setup). The archive from the release zip goes to `archive/Mac/mod/`; when it is there the plugin does not register it
again through ArchiveXL. `user.ini` (the saved values) is written next to the dylib, as on Windows.

## Diagnostics

| variable | effect |
|---|---|
| `MODSETTINGS_MAC_DIAG=1` | diagnostics, including every Bool default and how it was parsed |
| `MODSETTINGS_MAC_NOHOOKS=all` or `<name,...>` | leave hooks out (bisecting a crash) |

## Credits

Mod Settings is by jackhumbert (MIT). The macOS RED4ext SDK and loader are by jackmaxwil (MIT).
