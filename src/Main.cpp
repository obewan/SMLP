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
    if (int init = smlp->init(argc, argv); init == EXIT_FAILURE) {
      return EXIT_FAILURE;
    } else if (init == SimpleMLP::EXIT_HELP ||
               init == SimpleMLP::EXIT_VERSION) {
      return EXIT_SUCCESS;
    }

    switch (smlp->app_params.mode) {
    case EMode::Predictive:
      smlp->predict();
      break;
    case EMode::TrainOnly:
      smlp->train();
      break;
    case EMode::TestOnly:
      smlp->test();
      break;
    case EMode::TrainTestMonitored:
      smlp->trainTestMonitored();
      break;
    case EMode::TrainThenTest: {
      smlp->train();
      smlp->test();
    } break;
    default:
      throw std::runtime_error("Unimplemented mode.");
    }

    if (!smlp->app_params.network_to_export.empty()) {
      smlp->logger.info("Exporting network model to ",
                        smlp->app_params.network_to_export, "...");
      smlp->importExportJSON.exportModel(smlp->network.get(), smlp->app_params);
    }
  } catch (const std::exception &e) {
    smlp->logger.error(e.what());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}