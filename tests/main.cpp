// Unit tests for the parts of the plugin that need no game: ctest --test-dir build (or run mod_settings_tests.exe).
#include "Check.hpp"

int main() {
  int failed = 0;
  for (const auto &test : Check::Tests()) {
    const int before = Check::Failures();
    test.body();
    const bool ok = Check::Failures() == before;
    std::printf("%s %s\n", ok ? "ok  " : "FAIL", test.name);
    failed += ok ? 0 : 1;
  }
  std::printf("%zu tests, %d failed\n", Check::Tests().size(), failed);
  return failed;
}
