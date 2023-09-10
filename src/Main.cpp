#include "include/SimpleMLP.h"
#include <cstdlib>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments: 1:input_size 2:hidden_size
 * @code {.bash}
 * smlp -f ../test/mushroom/mushroom_data.csv -i 20 -s 20 -o 1 -e 10
 * -l 40000 -z false
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  auto smlp = new SimpleMLP();

  if (!smlp->init(argc, argv)) {
    std::cerr << "[ERROR] Init error. Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  if (!smlp->train()) {
    std::cerr << "[ERROR] Training error. Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  if (!smlp->test()) {
    std::cerr << "[ERROR] Testing error. Exiting." << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}