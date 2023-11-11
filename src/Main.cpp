#include "Common.h"
#include "include/SimpleMLP.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments
 * @code {.bash}
 * smlp -h
 * smlp -f ../data/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  auto smlp = std::make_unique<SimpleMLP>();

  try {
    int init = smlp->init(argc, argv);
    if (init == EXIT_FAILURE) {
      return EXIT_FAILURE;
    } else if (init == SimpleMLP::EXIT_HELP ||
               init == SimpleMLP::EXIT_VERSION) {
      return EXIT_SUCCESS;
    }

    smlp->runMode();

    if (smlp->shouldExportNetwork()) {
      smlp->exportNetwork();
    }
  } catch (const std::exception &e) {
    smlp->logger.error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}