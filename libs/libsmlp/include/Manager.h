/**
 * @file Manager.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief main manager
 * @date 2023-12-10
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2023
 *
 */
#pragma once

#include "Common.h"
#include "CommonModes.h"
#include "CommonResult.h"
#include "NetworkImportExport.h"
#include "Predict.h"
#include "RunnerVisitor.h"
#include "SimpleHTTPServer.h"
#include "SimpleLang.h"
#include "SimpleLogger.h"
#include "TestingFile.h"
#include "TestingSocket.h"
#include "TestingStdin.h"
#include "TrainingFile.h"
#include "TrainingSocket.h"
#include "TrainingStdin.h"
#include "exception/ManagerException.h"
#include <algorithm>
#include <memory>

namespace smlp {
class Manager {
public:
  static Manager &getInstance() {
    static Manager instance;
    return instance;
  }
  Manager(Manager const &) = delete;
  void operator=(Manager const &) = delete;

  /**
   * @brief Process line from TCP Client socket.
   *
   * @param line
   */
  smlp::Result processTCPClient(const std::string &line);

  /**
   * @brief run the selected mode.
   */
  void runMode();

  /**
   * @brief run a RunnerVisitor
   *
   * @param visitor
   */
  void runWithVisitor(const RunnerVisitor &visitor);

  /**
   * @brief run a RunnerVisitor for single line only
   *
   * @param visitor
   * @param line
   * @return Result
   */
  Result runWithLineVisitor(const RunnerVisitor &visitor,
                            const std::string &line);

  /**
   * @brief check if the network should export its model.
   */

  bool shouldExportNetwork() const;

  /**
   * @brief export the network model.
   */
  void exportNetwork() const;

  /**
   * @brief Network builder.
   */
  void importOrBuildNetwork();

  /**
   * @brief Application parameters.
   */
  AppParameters app_params;

  /**
   * @brief Network parameters.
   */
  NetworkParameters network_params;

  /**
   * @brief The neural network
   */
  std::unique_ptr<Network> network = nullptr;

  /**
   * @brief Http server
   *
   */
  std::unique_ptr<SimpleHTTPServer> http_server = nullptr;

  /**
   * @brief ImportExport tool.
   */
  NetworkImportExport importExport;

  /**
   * @brief Create a Http Server object
   *
   */
  void createHttpServer() {
    if (!app_params.enable_http && app_params.input != EInput::Socket) {
      throw ManagerException(SimpleLang::Error(Error::TCPSocketNotSet));
    }
    http_server = std::make_unique<SimpleHTTPServer>();
    http_server->setServerPort(app_params.http_port);
  }

  /**
   * @brief This will delete the managed object if this is the last shared_ptr
   * owning it.
   *
   */
  void resetHttpServer() {
    if (http_server == nullptr) {
      return;
    }
    http_server.reset();
  }

  /**
   * @brief Get a title line with the version
   *
   * @return std::string
   */
  std::string getVersionHeader() const;

  /**
   * @brief Get a detailled header line with the parameters
   *
   * @return std::string
   */
  std::string getInlineHeader() const;

private:
  Manager() = default;
};
} // namespace smlp