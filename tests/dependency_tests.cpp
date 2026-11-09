// How the ModSettings.dependency attribute splits into class, property and value (DependencyString.hpp).
#include "Check.hpp"

#include "DependencyString.hpp"

using namespace ModSettings;

namespace {

DependencyString Parse(const char *aText, bool aExpectOk = true) {
  DependencyString out;
  const bool ok = ParseDependency(aText, out);
  CHECK(ok == aExpectOk);
  return out;
}

} // namespace

TEST(property_alone_means_true) {
  auto d = Parse("visible");
  CHECK(d.className.empty());
  CHECK(d.propertyName == "visible");
  CHECK(d.value == "true");
}

TEST(property_with_value) {
  auto d = Parse("mode=Fast");
  CHECK(d.className.empty());
  CHECK(d.propertyName == "mode");
  CHECK(d.value == "Fast");
}

TEST(class_property_and_value) {
  auto d = Parse("MySettings.count=7");
  CHECK(d.className == "MySettings");
  CHECK(d.propertyName == "count");
  CHECK(d.value == "7");
}

TEST(class_and_property_without_value) {
  auto d = Parse("MySettings.enabled");
  CHECK(d.className == "MySettings");
  CHECK(d.propertyName == "enabled");
  CHECK(d.value == "true");
}

TEST(spaces_around_every_part) {
  auto d = Parse("  MySettings . count  =  7 ");
  CHECK(d.className == "MySettings");
  CHECK(d.propertyName == "count");
  CHECK(d.value == "7");
}

TEST(value_may_hold_periods_and_equals) {
  auto d = Parse("scale=1.5");
  CHECK(d.className.empty());
  CHECK(d.propertyName == "scale");
  CHECK(d.value == "1.5");

  auto e = Parse("label=a=b");
  CHECK(e.propertyName == "label");
  CHECK(e.value == "a=b");
}

TEST(empty_value_stays_empty) {
  auto d = Parse("name=");
  CHECK(d.propertyName == "name");
  CHECK(d.value.empty());
}

TEST(nothing_to_depend_on) {
  Parse("", false);
  Parse("   ", false);
  Parse("=true", false);
  Parse("MySettings.", false);
}
