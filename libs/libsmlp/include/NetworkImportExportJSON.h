/**
 * @file NetworkImportExportJSON.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Json import/export of the network meta data
 * @see https://github.com/nlohmann/json
 * @date 2023-10-29
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once
#include "Common.h"
#include "Layer.h"
#include "Network.h"
#include "OutputLayer.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "exception/ImportExportException.h"
#include "json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace smlp {
/**
 * @brief NetworkImportExportJSON class to export and import network models
 * using the JSON format.
 *
 */
class NetworkImportExportJSON {
public:
  /**
   * @brief Parse a json file into a network model.
   *
   * @param app_params the application parameters, including the file path/
   * @return Network*
   */
  Network *importModel(const AppParameters &app_params);

  /**
   * @brief export a Network model into a json file.
   *
   * @param network the network to export.
   * @param app_params the application parameters.
   */
  void exportModel(const Network *network,
                   const AppParameters &app_params) const;
};
} // namespace smlp