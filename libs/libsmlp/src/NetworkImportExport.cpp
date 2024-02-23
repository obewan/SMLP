#include "NetworkImportExport.h"
#include "exception/ImportExportException.h"
#include <exception>

using namespace smlp;

Network *NetworkImportExport::importModel(const AppParameters &app_params) {
  try {
    auto network = NIE_JSON.importModel(app_params);
    NIE_CSV.importNeuronsWeights(network, app_params);
    return network;
  } catch (std::exception &ex) {
    throw ImportExportException(ex.what());
  }
}

void NetworkImportExport::exportModel(const Network *network,
                                      const AppParameters &app_params) const {
  try {
    NIE_JSON.exportModel(network, app_params);
    NIE_CSV.exportNeuronsWeights(network, app_params);
  } catch (std::exception &ex) {
    throw ImportExportException(ex.what());
  }
}