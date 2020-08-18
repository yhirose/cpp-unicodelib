// This file is to check duplicate symbols
#include "catch.hpp"
#include <unicodelib.h>

TEST_CASE("Duplicate Check", "[duplicate]") {
  REQUIRE(unicode::is_white_space(U'a') == false);
}
