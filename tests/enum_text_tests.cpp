// A list variable's saved text as an index (EnumText.hpp): its value names, a Bool saved before under the same name,
// and numbers.
#include "Check.hpp"

#include "EnumText.hpp"

using namespace ModSettings;

namespace {
const std::vector<std::string> kNames{"Hover", "Wheels", "TouchDown"};
}

TEST(enum_text_value_names) {
  CHECK(EnumIndexFromText("Hover", kNames, 1) == 0);
  CHECK(EnumIndexFromText("TouchDown", kNames, 1) == 2);
}

TEST(enum_text_saved_bool_is_the_first_two) {
  CHECK(EnumIndexFromText("false", kNames, 2) == 0);
  CHECK(EnumIndexFromText("true", kNames, 2) == 1);
  CHECK(EnumIndexFromText("true", {"Only"}, 0) == 0);
}

TEST(enum_text_numbers_in_range) {
  CHECK(EnumIndexFromText("0", kNames, 1) == 0);
  CHECK(EnumIndexFromText("2", kNames, 1) == 2);
  CHECK(EnumIndexFromText("3", kNames, 1) == 1);
  CHECK(EnumIndexFromText("-1", kNames, 1) == 1);
  CHECK(EnumIndexFromText("99999999999", kNames, 1) == 1);
}

TEST(enum_text_anything_else_keeps_the_fallback) {
  CHECK(EnumIndexFromText("", kNames, 1) == 1);
  CHECK(EnumIndexFromText("hover", kNames, 2) == 2);
}
