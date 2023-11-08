#include "Network.h"
#include "doctest.h"

TEST_CASE("Testing the Network class") {
  SUBCASE("Test Constructor and Destructor") {
    Network *network = nullptr;
    CHECK_NOTHROW(network = new Network());
    CHECK_NOTHROW(delete network);
    CHECK_NOTHROW(network = new Network({.input_size = 10,
                                         .hidden_size = 3,
                                         .output_size = 2,
                                         .hiddens_count = 2}));
    CHECK_NOTHROW(delete network);
  }
}