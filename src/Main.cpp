#include "Common.h"
#include "include/SimpleMLP.h"
#include <cstdlib>
#include <iostream>
#include <ostream>

/**
 * @brief main function
 *
 * @param argc numbers of arguments
 * @param argv table of arguments: 1:input_size 2:hidden_size
 * @code {.bash}
 * smlp -h
 * smlp -f ../test/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100 -t
 * 0.7 -r 0.01
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  auto smlp = new SimpleMLP();

  if (bool showVersion{false}; !smlp->init(argc, argv, showVersion)) {
    if (showVersion) {
      std::cout << smlp->app_params.title << " v" << smlp->app_params.version
                << std::endl;
      std::cout << "Copyright Damien Balima (https://dams-labs.net) 2023"
                << std::endl;
    }
    return EXIT_FAILURE;
  }

  switch (smlp->app_params.mode) {
  case EMode::TrainOnly: {
    smlp->train();
  } break;
  case EMode::TestOnly: {
    smlp->test();
  } break;
  case EMode::TrainTestMonitored: {
    smlp->trainTestMonitored();
  } break;
  // case Mode::TrainThenTest:
  default: {
    smlp->train();
    smlp->test();
  } break;
  }

  if (!smlp->app_params.network_to_export.empty()) {
    std::cout << "[INFO] Exporting network model to "
              << smlp->app_params.network_to_export << "..." << std::endl;
    smlp->importExportJSON.exportModel(smlp->network, smlp->app_params);
  }

  return EXIT_SUCCESS;
}