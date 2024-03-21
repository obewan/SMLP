#include "NetworkImportExport.h"
#include "NetworkImportExportCSV.h"
#include "NetworkImportExportJSON.h"
#include "exception/ImportExportException.h"
#include <exception>

using namespace smlp;

std::unique_ptr<Network>
NetworkImportExport::importModel(const AppParameters &app_params) {
  try {
    NetworkImportExportCSV NIE_CSV;
    NetworkImportExportJSON NIE_JSON;
    auto network = NIE_JSON.importModel(app_params);
    NIE_CSV.importNeuronsWeights(network, app_params);
    return network;
  } catch (std::exception &ex) {
    throw ImportExportException(ex.what());
  }
}

void NetworkImportExport::exportModel(const std::unique_ptr<Network> &network,
                                      const AppParameters &app_params) const {
  try {
    NetworkImportExportCSV NIE_CSV;
    NetworkImportExportJSON NIE_JSON;
    NIE_JSON.exportModel(network, app_params);
    NIE_CSV.exportNeuronsWeights(network, app_params);
  } catch (std::exception &ex) {
    throw ImportExportException(ex.what());
  }
}