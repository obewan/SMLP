#include "Network.h"
#include "doctest.h"

TEST_CASE("Testing the Network class") {
  SUBCASE("Test Constructor and Destructor") {
    Network *network = nullptr;
    CHECK_NOTHROW(network = new Network());
    CHECK_NOTHROW(delete network);
  }
}