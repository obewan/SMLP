#include "NetworkImportExportFacade.h"
#include "exception/ImportExportException.h"
#include <exception>

using namespace smlp;

std::unique_ptr<Network>
NetworkImportExportFacade::importModel(const AppParameters &app_params) {
  try {
    auto network = jsonIE.importModel(app_params);
    csvIE.importNeuronsWeights(network, app_params);
    return network;
  } catch (std::exception &ex) {
    throw ImportExportException(ex.what());
  }
}

void NetworkImportExportFacade::exportModel(
    const std::unique_ptr<Network> &network,
    const AppParameters &app_params) const {
  try {
    jsonIE.exportModel(network, app_params);
    csvIE.exportNeuronsWeights(network, app_params);
  } catch (std::exception &ex) {
    throw ImportExportException(ex.what());
  }
}