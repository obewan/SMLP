#include "exception/FileParserException.h"
#include <bits/types/FILE.h>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Common.h"
#include "DataFileParser.h"
#include "Manager.h"
#include "doctest.h"
#include <cmath>
#include <string>

TEST_CASE("Testing the DataFileParser class") {
  std::string test_file = "../data/test_file.csv";
  const float eps = 1e-6f; // epsilon for float testing

  SUBCASE("Test Constructor") { CHECK_NOTHROW(DataFileParser()); }
  DataFileParser parser;

  SUBCASE("Test openFile") {
    Manager::getInstance().app_params.data_file =
        "oops"; // This file does not exist
    CHECK_THROWS_AS(parser.openFile(), FileParserException);

    Manager::getInstance().app_params.data_file = test_file;
    CHECK_NOTHROW(parser.openFile());
    CHECK(parser.file.is_open() == true);
    parser.closeFile();
  }

  SUBCASE("Test closeFile") {
    parser.openFile();
    parser.closeFile();
    CHECK(parser.file.is_open() == false);
  }

  SUBCASE("Test resetPos") {
    parser.openFile();
    CHECK(parser.file.is_open() == true);
    std::string line;
    std::getline(parser.file, line);
    CHECK((long)parser.file.tellg() > 0);
    parser.resetPos();
    CHECK((long)parser.file.tellg() == 0);
    parser.closeFile();
  }

  SUBCASE("Test getTrainingRatioLine") {
    CHECK(parser.isTrainingRatioLineProcessed == false);
    parser.calcTrainingRatioLine(
        {.training_ratio = 0.6f, .training_ratio_line = 0, .use_stdin = false});
    CHECK(parser.training_ratio_line == 6);
    CHECK(parser.total_lines == 10);
    CHECK(parser.isTrainingRatioLineProcessed == true);
  }

  SUBCASE("Test countLine") {
    CHECK(parser.file.is_open() == false);
    auto linesCount = parser.countLine();
    CHECK(linesCount == 10);
    CHECK(parser.file.is_open() == false);
  }

  SUBCASE("Test processLine") {
    Manager::getInstance().network_params = {.input_size = 20,
                                             .hidden_size = 12,
                                             .output_size = 1,
                                             .hiddens_count = 1};
    Manager::getInstance().app_params = {.data_file = test_file,
                                         .output_at_end = false};

    parser.openFile();

    // Test first line
    RecordResult result = parser.processLine();
    CHECK(result.isSuccess == true);
    const auto &[inputs, outputs] = result.record;
    std::vector<float> expectedInputs = {
        0.08f, 0.43f, 0.80f, 0.08f, 1.00f, 0.75f, 0.67f, 0.69f, 0.15f, 0.09f,
        0.00f, 0.36f, 0.08f, 0.00f, 0.00f, 1.00f, 0.92f, 0.00f, 0.62f, 0.00f};
    std::vector<float> expectedOuputs = {1.00f};

    CHECK(inputs.size() == expectedInputs.size());
    CHECK(outputs.size() == expectedOuputs.size());
    for (size_t i = 0; i < inputs.size(); ++i) {
      CHECK(inputs[i] == doctest::Approx(expectedInputs[i]).epsilon(eps));
    }
    for (size_t i = 0; i < outputs.size(); ++i) {
      CHECK(outputs[i] == doctest::Approx(expectedOuputs[i]).epsilon(eps));
    }

    // Test next line
    RecordResult result2 = parser.processLine();
    CHECK(result.isSuccess == true);
    const auto &[inputs2, outputs2] = result2.record;
    std::vector<float> expectedInputs2 = {
        0.09f, 0.43f, 0.40f, 0.08f, 1.00f, 0.50f, 0.33f, 0.62f, 0.15f, 0.07f,
        0.00f, 0.09f, 0.62f, 0.00f, 0.00f, 1.00f, 0.92f, 0.42f, 1.00f, 0.0f};
    std::vector<float> expectedOuputs2 = {1.00f};

    CHECK(inputs2.size() == expectedInputs2.size());
    CHECK(outputs2.size() == expectedOuputs2.size());
    for (size_t i = 0; i < inputs2.size(); ++i) {
      CHECK(inputs2[i] == doctest::Approx(expectedInputs2[i]).epsilon(eps));
    }
    for (size_t i = 0; i < outputs2.size(); ++i) {
      CHECK(outputs2[i] == doctest::Approx(expectedOuputs2[i]).epsilon(eps));
    }

    // Test special Input Only of Predictive mode:
    // in Predictive mode outputs will be
    // predicted and added at the end, so they are
    // optional in dataset entry, but inputs are mandatory ofc.
    parser.closeFile();
    Manager::getInstance().app_params.mode = EMode::Predictive;
    Manager::getInstance().app_params.data_file =
        "../data/test_file_input_only.csv";
    parser.openFile();
    CHECK_NOTHROW(parser.processLine());
    RecordResult result3 = parser.processLine();
    const auto &[inputs3, outputs3] = result3.record;
    CHECK(inputs3.size() == Manager::getInstance().network_params.input_size);
    CHECK(outputs3.size() == 0);
    parser.closeFile();
  }

  SUBCASE("Test processInputFirst and processOutputFirst") {
    std::vector<std::vector<Csv::CellReference>>
        cell_refs; // Assuming you have some cell_refs
    size_t input_size = 20;
    size_t output_size = 1;
    std::string line =
        "1.00,0.04,0.57,0.80,0.08,1.00,0.38,0.00,0.85,0.12,0.05,0.00,"
        "0.73,0.62,0.00,0.00,1.00,0.92,0.00,1.00,0.00";
    std::string_view data(line);
    parser.csv_parser.parseTo(data, cell_refs);

    const auto &[inputs1, outputs1] =
        parser.processInputFirst(cell_refs, input_size, output_size);
    const auto &[inputs2, outputs2] =
        parser.processOutputFirst(cell_refs, input_size, output_size);

    CHECK(inputs1.size() == input_size);
    CHECK(outputs1.size() == output_size);
    CHECK(inputs2.size() == input_size);
    CHECK(outputs2.size() == output_size);

    CHECK(inputs1.at(0) == doctest::Approx(1.00f).epsilon(eps));
    CHECK(inputs2.at(0) == doctest::Approx(0.04f).epsilon(eps));
    CHECK(outputs1.at(0) == doctest::Approx(0.00f).epsilon(eps));
    CHECK(outputs2.at(0) == doctest::Approx(1.00f).epsilon(eps));
  }

  SUBCASE("Test processLine errors") {
    parser.closeFile();
    auto &manager = Manager::getInstance();
    manager.network_params = {.input_size = 20,
                              .hidden_size = 12,
                              .output_size = 1,
                              .hiddens_count = 1};
    manager.app_params = {.output_at_end = false};

    manager.app_params.data_file = "../data/test_file_bad_line.csv";
    parser.openFile();
    CHECK_NOTHROW(parser.processLine());
    CHECK_THROWS_WITH_AS(
        parser.processLine(),
        "CSV parsing error at line 2: there are 20 columns instead of 21.",
        FileParserException);
    parser.closeFile();

    manager.app_params.data_file = "../data/test_file_bad_format.csv";
    parser.openFile();
    CHECK_THROWS_WITH_AS(parser.processLine(),
                         "CSV parsing error at line 1: bad format.",
                         FileParserException);
    parser.closeFile();

    // Special Predictive mode tests
    manager.app_params.mode = EMode::Predictive;
    manager.app_params.data_file = "../data/test_file_short_line.csv";
    parser.openFile();
    CHECK_THROWS_WITH_AS(
        parser.processLine(),
        "CSV parsing error at line 1: there are 19 columns instead "
        "of a minimum of 20.",
        FileParserException);
  }
}
