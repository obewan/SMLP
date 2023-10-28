#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "FileParser.h"
#include "doctest.h"
#include <cmath>
#include <string>

TEST_CASE("Testing the FileParser class") {
  SUBCASE("Test Constructor") { CHECK_NOTHROW(FileParser("")); }

  // beware current path is "build/libs/libsmlp/test"
  std::string test_file = "../../../../libs/libsmlp/test/data/test_file.csv";

  SUBCASE("Test openFile") {
    FileParser parser_NOK("non_existent_file.csv"); // This file does not exist
    CHECK_THROWS_AS(parser_NOK.openFile(), FileParserException);

    FileParser parser_OK(test_file);
    CHECK_NOTHROW(parser_OK.openFile());
    CHECK(parser_OK.file.is_open() == true);
    parser_OK.closeFile();
  }

  FileParser parser(test_file);

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
    auto trainingRatioLine = parser.getTrainingRatioLine(0.6f);
    CHECK(trainingRatioLine == 6);
    CHECK(parser.training_ratio_line == trainingRatioLine);
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
    Parameters params{.input_size = 20,
                      .hidden_size = 12,
                      .output_size = 1,
                      .hiddens_count = 1,
                      .output_at_end = true};

    CHECK(params.input_size == 20);
    CHECK(params.hidden_size == 12);

    parser.openFile();

    // Test first line
    RecordResult result = parser.processLine(params);
    CHECK(result.isSuccess == true);
    const auto &[inputs, outputs] = result.record;
    std::vector<float> expectedInputs = {
        1.00f, 0.08f, 0.43f, 0.80f, 0.08f, 1.00f, 0.75f, 0.67f, 0.69f, 0.15f,
        0.09f, 0.00f, 0.36f, 0.08f, 0.00f, 0.00f, 1.00f, 0.92f, 0.00f, 0.62f};
    std::vector<float> expectedOuputs = {0.0f};

    CHECK(inputs.size() == expectedInputs.size());
    CHECK(outputs.size() == expectedOuputs.size());
    for (size_t i = 0; i < inputs.size(); ++i) {
      // using absolute difference because of float precision issue
      CHECK(std::fabs(inputs[i] - expectedInputs[i]) <= 1e-6f);
    }
    for (size_t i = 0; i < outputs.size(); ++i) {
      CHECK(std::fabs(outputs[i] - expectedOuputs[i]) <= 1e-6f);
    }

    // Test next line
    RecordResult result2 = parser.processLine(params);
    CHECK(result.isSuccess == true);
    const auto &[inputs2, outputs2] = result2.record;
    std::vector<float> expectedInputs2 = {
        1.00f, 0.09f, 0.43f, 0.40f, 0.08f, 1.00f, 0.50f, 0.33f, 0.62f, 0.15f,
        0.07f, 0.00f, 0.09f, 0.62f, 0.00f, 0.00f, 1.00f, 0.92f, 0.42f, 1.00f};
    std::vector<float> expectedOuputs2 = {0.0f};

    CHECK(inputs2.size() == expectedInputs2.size());
    CHECK(outputs2.size() == expectedOuputs2.size());
    for (size_t i = 0; i < inputs2.size(); ++i) {
      // using absolute difference because of float precision issue
      CHECK(std::fabs(inputs2[i] - expectedInputs2[i]) <= 1e-6f);
    }
    for (size_t i = 0; i < outputs2.size(); ++i) {
      CHECK(std::fabs(outputs2[i] - expectedOuputs2[i]) <= 1e-6f);
    }
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
        parser.processInputFirst(cell_refs, input_size);
    const auto &[inputs2, outputs2] =
        parser.processOutputFirst(cell_refs, output_size);

    CHECK(inputs1.size() == input_size);
    CHECK(outputs1.size() == output_size);
    CHECK(inputs2.size() == input_size);
    CHECK(outputs2.size() == output_size);
    // using absolute difference because of float precision issue
    CHECK(std::fabs(inputs1.at(0) - 1.00f) <= 1e-6f);
    CHECK(std::fabs(inputs2.at(0) - 0.04f) <= 1e-6f);
    CHECK(std::fabs(outputs1.at(0) - 0.00f) <= 1e-6f);
    CHECK(std::fabs(outputs2.at(0) - 1.00f) <= 1e-6f);
  }
}
