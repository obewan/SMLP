/**
 * @file NetworkImportExport.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Import/export of the neural network
 * @date 2024-02-20
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "Network.h"
#include <memory.h>

namespace smlp {
class NetworkImportExport {
public:
  virtual ~NetworkImportExport() = default;

  /**
   * @brief Import a network model files (JSON meta data and CSV neurons data)
   *
   * @param app_params
   * @return Network*
   */
  virtual std::unique_ptr<Network> importModel(const AppParameters &app_params);

  /**
   * @brief Export a network model files (JSON meta data and CSV neurons data)
   *
   * @param network
   * @param app_params
   */
  virtual void exportModel(const std::unique_ptr<Network> &network,
                           const AppParameters &app_params) const;
};
} // namespace smlp