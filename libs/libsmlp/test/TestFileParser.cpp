#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/FileParser.h"
#include "doctest.h"

TEST_CASE("Testing FileParser") { CHECK_NOTHROW(FileParser("")); }
