// This file is to check duplicate symbols
#include <catch2/catch_test_macros.hpp>
#include <unicodelib.h>

TEST_CASE("Duplicate Check", "[duplicate]") {
  REQUIRE(unicode::is_white_space(U'a') == false);
}
