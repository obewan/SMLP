#include "Common.h"
#include "CommonErrors.h"
#include "CommonResult.h"
#include "doctest.h"

TEST_CASE("Testing the Common class") {

  SUBCASE("Test getStrings") {
    for (auto mode :
         {EMode::TestOnly, EMode::TrainOnly, EMode::TrainTestMonitored,
          EMode::TrainThenTest, EMode::Predict}) {
      CHECK(!smlp::getModeStr(mode).empty());
    }

    for (auto pred : {EPredictMode::CSV, EPredictMode::NumberAndRaw,
                      EPredictMode::NumberOnly, EPredictMode::RawOnly}) {
      CHECK(!smlp::getPredictModeStr(pred).empty());
    }

    for (auto activ :
         {EActivationFunction::ELU, EActivationFunction::LReLU,
          EActivationFunction::PReLU, EActivationFunction::ReLU,
          EActivationFunction::Sigmoid, EActivationFunction::Tanh}) {
      CHECK(!smlp::getActivationStr(activ).empty());
    }
  }

  SUBCASE("Test trims") {
    CHECK(smlp::ltrim("  test  ") == "test  ");
    CHECK(smlp::rtrim("  test  ") == "  test");
    CHECK(smlp::trim("  test  ") == "test");
    CHECK(smlp::trimCRLF("\r\ntest") == "test");
    CHECK(smlp::trimCRLF("\r\ntest\r\n") == "test");
    CHECK(smlp::trimCRLF("test\r\n") == "test");
    CHECK(smlp::trimRNULL("test\0") == "test");
    CHECK(smlp::trimALL("\r\n  test \r\n\0") == "test");
  }

  SUBCASE("Test Result") {
    using namespace smlp;
    Result result;
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Failure");
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":1,\"data\":\"\",\"message\":\"Failure\"}");

    result.code = smlp::make_error_code(ErrorCode::Success);
    result.action = smlp::getModeStr(EMode::Predict);
    CHECK(result.isSuccess() == true);
    CHECK(result.message() == "Success");
    CHECK(result.json() == "{\"action\":\"Predict\",\"code\":0,\"data\":\"\","
                           "\"message\":\"Success\"}");

    result.code = smlp::make_error_code(ErrorCode::OK);
    result.action = smlp::getModeStr(EMode::TestOnly);
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == true);
    CHECK(result.message() == "OK");
    CHECK(
        result.json() ==
        R"({"action":"TestOnly","code":200,"data":"{\"test\":\"ok\"}","message":"OK"})");

    result.code = smlp::make_error_code(ErrorCode::BadRequest);
    result.action = "";
    result.data = R"({"test":"badrequest"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Bad Request");
    CHECK(
        result.json() ==
        R"({"action":"","code":400,"data":"{\"test\":\"badrequest\"}","message":"Bad Request"})");

    result.code = smlp::make_error_code(ErrorCode::InternalServerError);
    result.action = "";
    result.data = R"({"test":"ISE"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Internal Server Error");
    CHECK(
        result.json() ==
        R"({"action":"","code":500,"data":"{\"test\":\"ISE\"}","message":"Internal Server Error"})");

    result.code = std::make_error_code(std::errc::file_exists);
    result.action = "";
    result.data = R"({"test":"file exists"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "File exists");
    CHECK(
        result.json() ==
        R"({"action":"","code":17,"data":"{\"test\":\"file exists\"}","message":"File exists"})");
  }
}