macOS copy of deps/red_lib/include (jackhumbert/cp2077-red-lib, "jack" branch, commit 2d9bb93, MIT).
Why a copy: RedLib's headers include each other with quoted relative paths, so a patched header cannot shadow the submodule's one
from an include path. The submodule is not edited. Every changed line is marked MAC (grep -rn "MAC" macos/redlib).
Patches follow the Mac fixes already in cp2077-codeware/lib/Red.
