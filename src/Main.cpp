#include "SimpleLogger.h"
#include "include/SimpleMLP.h"
#include <cstdlib>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments
 * @code {.bash}
 * smlp -h
 * smlp -f ../data/mushroom/mushroom_data.csv -s 20 -o 1 -d 12 -c 1 -p 100
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  auto smlp = std::make_unique<SimpleMLP>();

  try {
    int init = smlp->init(argc, argv);
    if (init == SimpleMLP::EXIT_HELP || init == SimpleMLP::EXIT_VERSION) {
      return EXIT_SUCCESS;
    } else if (init != EXIT_SUCCESS) {
      return init;
    }

    smlp->run();

  } catch (const std::exception &e) {
    smlp::SimpleLogger::getInstance().error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}