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
 * @param argv table of arguments: 1:input_size 2:hidden_size
 * @code {.bash}
 * smlp -h
 * smlp -f ../data/mushroom/mushroom_data.csv -i 20 -o 1 -d 12 -c 1 -e 100 -t
 * 0.7 -r 0.01
 * @endcode
 *
 * @return int
 */
int main(int argc, char *argv[]) {
  auto smlp = std::make_unique<SimpleMLP>();

  if (bool showVersion{false}; !smlp->init(argc, argv, showVersion)) {
    if (showVersion) {
      smlp->logger.out(smlp->app_params.title, " v", smlp->app_params.version);
      smlp->logger.out("Copyright Damien Balima (https://dams-labs.net) 2023");
      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
  }

  try {
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