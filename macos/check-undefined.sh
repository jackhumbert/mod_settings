#!/bin/bash
# After the link: (1) the three RED4ext entry points are exported, (2) the dylib has no symbol that is looked up dynamically.
# The link flag -undefined dynamic_lookup (needed because address hashes, not symbol names, reach the game) would also hide a function
# of this plugin that is declared and never compiled in; the first call of it would crash. Everything this plugin calls in the game goes
# through an address, so the list of dynamically looked up symbols must be empty (macos/undefined-allowed.txt can name exceptions).
# Usage: check-undefined.sh <path to mod_settings.dylib>
set -eu
DYLIB="$1"
ALLOW="$(dirname "$0")/undefined-allowed.txt"
for sym in _Main _Query _Supports; do
  nm -gU "$DYLIB" | grep -q " T $sym\$" || { echo "check-undefined: FAILED. export $sym is missing" >&2; exit 1; }
done
BAD=$(nm -m -u "$DYLIB" | grep "dynamically looked up" | sed -E 's/^ +\(undefined\) external //; s/ \(dynamically looked up\)$//' || true)
if [[ -f "$ALLOW" && -n "$BAD" ]]; then
  BAD=$(echo "$BAD" | grep -vxFf <(grep -v '^#' "$ALLOW" | grep -v '^$') || true)
fi
if [[ -n "$BAD" ]]; then
  echo "check-undefined: FAILED. Symbols looked up dynamically at load (declared, not compiled in):" >&2
  echo "$BAD" | c++filt >&2
  exit 1
fi
echo "check-undefined: ok (Main, Query, Supports exported; no dynamically looked up symbols)"
