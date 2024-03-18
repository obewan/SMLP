#include "NetworkImportExportCSV.h"
#include "csv_parser.h"
#include "exception/EmptyCellException.h"
#include "exception/ImportExportException.h"
#include <algorithm> // for std::transform
#include <cctype>    // for std::tolower
#include <cstddef>
#include <exception>
#include <fstream>
#include <regex> // for std::regex and std::regex_replace
#include <string>

using namespace smlp;

void NetworkImportExportCSV::importNeuronsWeights(
    const Network *network, const AppParameters &app_params) const {
  // lambda function to convert to float
  auto getFloatValue = [](const std::vector<Csv::CellReference> &cells) {
    auto val = cells[0].getDouble();
    if (val.has_value()) {
      return static_cast<float>(val.value());
    } else {
      throw EmptyCellException();
    }
  };

  // lambda function to add cell value to the weights vector
  auto processCell =
      [&getFloatValue](std::vector<float> &weights,
                       const std::vector<Csv::CellReference> &cells) {
        try {
          weights.push_back(getFloatValue(cells));
        } catch (EmptyCellException &) {
          // ignore the exception caused by an empty cell
          return;
        }
      };

  // get the csv filename
  std::string filename = app_params.network_to_import;
  filename = std::regex_replace(
      filename, std::regex(".json$", std::regex::icase), ".csv");
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw ImportExportException(SimpleLang::Error(Error::FailedToOpenFile) +
                                ": " + filename);
  }

  // parsing the csv
  Csv::Parser csv_parser;
  std::string line;
  int current_line_number = 0;
  bool header_skipped = false;
  while (std::getline(file, line)) {
    current_line_number++;
    if (!header_skipped) {
      header_skipped = true;
      continue;
    }
    std::vector<std::vector<Csv::CellReference>> cell_refs;

    try {
      std::string_view data(line);
      csv_parser.parseTo(data, cell_refs);
    } catch (Csv::ParseError &ex) {
      throw ImportExportException(
          SimpleLang::Error(
              Error::CSVParsingError,
              {{"line_number", std::to_string(current_line_number)}}) +
          ": " + ex.what());
    }

    if (cell_refs.empty() || cell_refs.size() < 2) {
      throw ImportExportException(SimpleLang::Error(
          Error::CSVParsingErrorEmptyLine,
          {{"line_number", std::to_string(current_line_number)}}));
    }
    if (cell_refs.size() == 2) {
      continue;
    }

    try {
      auto layer_index = (size_t)getFloatValue(cell_refs.at(0));
      auto neuron_index = (size_t)getFloatValue(cell_refs.at(1));
      std::vector<float> weights;

      std::for_each(cell_refs.begin() + 2, cell_refs.end(),
                    std::bind_front(processCell, std::ref(weights)));

      network->layers.at(layer_index)
          ->neurons.at(neuron_index)
          .weights.swap(weights);
    } catch (std::exception &ex) {
      throw ImportExportException(ex.what());
    }
  }

  file.close();
}

void NetworkImportExportCSV::exportNeuronsWeights(
    const Network *network, const AppParameters &app_params) const {
  // get the csv filename
  std::string filename = app_params.network_to_export;
  filename = std::regex_replace(
      filename, std::regex(".json$", std::regex::icase), ".csv");
  std::ofstream file(filename);

  // Determine the maximum number of weights any neuron has
  size_t max_weights = 0;
  for (const auto &layer : network->layers) {
    for (const auto &neuron : layer->neurons) {
      if (neuron.weights.size() > max_weights) {
        max_weights = neuron.weights.size();
      }
    }
  }

  // Write the header to the CSV file
  file << "Layer,Neuron";
  for (size_t i = 0; i < max_weights; ++i) {
    file << ",Weight" << (i + 1);
  }
  file << "\n";

  // Write the data
  for (size_t layer_index = 0; layer_index < network->layers.size();
       layer_index++) {
    const auto &layer = network->layers.at(layer_index);
    for (size_t neuron_index = 0; neuron_index < layer->neurons.size();
         neuron_index++) {
      const auto &neuron = layer->neurons.at(neuron_index);

      // Write the layer index and neuron index to the CSV file
      file << layer_index << "," << neuron_index;

      // Write the weights to the CSV file
      for (size_t i = 0; i < max_weights; ++i) {
        if (i < neuron.weights.size()) {
          file << "," << neuron.weights[i];
        } else {
          // If the neuron doesn't have a weight for this index, write a blank
          // column
          file << ",";
        }
      }

      file << "\n";
    }
  }

  file.close();
}