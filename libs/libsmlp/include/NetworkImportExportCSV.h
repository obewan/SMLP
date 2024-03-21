/**
 * @file NetworkImportExportCSV.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief CSV import/export of the network neurons
 * @date 2024-02-20
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "CommonParameters.h"
#include "Network.h"

namespace smlp {
class NetworkImportExportCSV {
public:
  /**
   * @brief Import the network neurons data from a CSV file.
   *
   * @param network
   * @param app_params
   */
  void importNeuronsWeights(std::unique_ptr<Network> &network,
                            const AppParameters &app_params) const;

  /**
   * @brief Export the network neurons data to a CSV file.
   *
   * @param network
   * @param app_params
   */
  void exportNeuronsWeights(const std::unique_ptr<Network> &network,
                            const AppParameters &app_params) const;
};
} // namespace smlp