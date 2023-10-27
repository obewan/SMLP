#include "include/SimpleMLP.h"
#include <cstdlib>
#include <iostream>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments: 1:input_size 2:hidden_size
 * @code {.bash}
 * smlp -h
 * smlp -f ../test/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100 -t
 * 0.7 -r 0.01 -v false
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  auto smlp = new SimpleMLP();

  if (!smlp->init(argc, argv)) {
    return EXIT_FAILURE;
  }

  // can also use just smlp->train() and/or smlp->test()
  // but trainAndTest() will log the progress as it will test at each epoch.
  smlp->trainAndTestMonitored();

  return EXIT_SUCCESS;
}