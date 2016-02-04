#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <unicodelib.h>

using namespace std;
namespace ucl = unicodelib;

TEST_CASE("Sample test", "[general]") {
    auto text = u8"日本語もOKです。";
    auto s32 = ucl::to_u32string(u8"日本語もOKです。");
    auto s8 = ucl::to_u8string(s32);
    REQUIRE(text == s8);
}
