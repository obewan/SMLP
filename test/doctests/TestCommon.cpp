#include "Common.h"
#include "CommonErrors.h"
#include "CommonResult.h"
#include "doctest.h"

using namespace smlp;

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

  SUBCASE("Test getEscapedString") {
    std::string test = "that's \r\na string\r\n";
    CHECK(smlp::getEscapedString(test) == "that's \\r\\na string\\r\\n");
  }

  SUBCASE("Test Result") {
    using namespace smlp;
    Result result;
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Failure");
    CHECK(result.code.value() == 1);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":1,\"data\":\"\",\"message\":\"Failure\"}");

    result.code = smlp::make_error_code(ErrorCode::Success);
    result.action = smlp::getModeStr(EMode::Predict);
    CHECK(result.isSuccess() == true);
    CHECK(result.code.value() == 0);
    CHECK(result.message() == "Success");
    CHECK(result.json() == "{\"action\":\"Predict\",\"code\":0,\"data\":\"\","
                           "\"message\":\"Success\"}");

    result.code = smlp::make_error_code(ErrorCode::OK);
    result.action = smlp::getModeStr(EMode::TestOnly);
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == true);
    CHECK(result.code.value() == 200);
    CHECK(result.message() == "OK");
    CHECK(result.json() == "{\"action\":\"TestOnly\",\"code\":200,\"data\":\"{"
                           "\\\"test\\\":\\\"ok\\\"}\",\"message\":\"OK\"}");

    result.code = smlp::make_error_code(ErrorCode::Created);
    result.action = smlp::getModeStr(EMode::TestOnly);
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == true);
    CHECK(result.message() == "Created");
    CHECK(result.code.value() == 201);
    CHECK(result.json() ==
          "{\"action\":\"TestOnly\",\"code\":201,\"data\":\"{\\\"test\\\":"
          "\\\"ok\\\"}\",\"message\":\"Created\"}");

    result.code = smlp::make_error_code(ErrorCode::Accepted);
    result.action = smlp::getModeStr(EMode::TestOnly);
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == true);
    CHECK(result.message() == "Accepted");
    CHECK(result.code.value() == 202);
    CHECK(result.json() ==
          "{\"action\":\"TestOnly\",\"code\":202,\"data\":\"{\\\"test\\\":"
          "\\\"ok\\\"}\",\"message\":\"Accepted\"}");

    result.code = smlp::make_error_code(ErrorCode::BadRequest);
    result.action = "";
    result.data = R"({"test":"badrequest"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Bad Request");
    CHECK(result.code.value() == 400);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":400,\"data\":\"{\\\"test\\\":"
          "\\\"badrequest\\\"}\",\"message\":\"Bad Request\"}");

    result.code = smlp::make_error_code(ErrorCode::Unauthorized);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Unauthorized");
    CHECK(result.code.value() == 401);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":401,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Unauthorized\"}");

    result.code = smlp::make_error_code(ErrorCode::PaymentRequired);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Payment Required");
    CHECK(result.code.value() == 402);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":402,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Payment Required\"}");

    result.code = smlp::make_error_code(ErrorCode::Forbidden);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Forbidden");
    CHECK(result.code.value() == 403);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":403,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Forbidden\"}");

    result.code = smlp::make_error_code(ErrorCode::NotFound);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Not Found");
    CHECK(result.code.value() == 404);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":404,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Not Found\"}");

    result.code = smlp::make_error_code(ErrorCode::MethodNotAllowed);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Method Not Allowed");
    CHECK(result.code.value() == 405);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":405,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Method Not Allowed\"}");

    result.code = smlp::make_error_code(ErrorCode::NotAcceptable);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Not Acceptable");
    CHECK(result.code.value() == 406);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":406,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Not Acceptable\"}");

    result.code = smlp::make_error_code(ErrorCode::ProxyAuthenticationRequired);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Proxy Authentication Required");
    CHECK(result.code.value() == 407);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":407,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Proxy Authentication Required\"}");

    result.code = smlp::make_error_code(ErrorCode::RequestTimeout);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Request Timeout");
    CHECK(result.code.value() == 408);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":408,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Request Timeout\"}");

    result.code = smlp::make_error_code(ErrorCode::Conflict);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Conflict");
    CHECK(result.code.value() == 409);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":409,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Conflict\"}");

    result.code = smlp::make_error_code(ErrorCode::InternalServerError);
    result.action = "";
    result.data = R"({"test":"ISE"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Internal Server Error");
    CHECK(result.code.value() == 500);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":500,\"data\":\"{\\\"test\\\":\\\"ISE\\\"}"
          "\",\"message\":\"Internal Server Error\"}");

    result.code = smlp::make_error_code(ErrorCode::NotImplemented);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Not Implemented");
    CHECK(result.code.value() == 501);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":501,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Not Implemented\"}");

    result.code = smlp::make_error_code(ErrorCode::BadGateway);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Bad Gateway");
    CHECK(result.code.value() == 502);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":502,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Bad Gateway\"}");

    result.code = smlp::make_error_code(ErrorCode::ServiceUnavailable);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Service Unavailable");
    CHECK(result.code.value() == 503);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":503,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Service Unavailable\"}");

    result.code = smlp::make_error_code(ErrorCode::GatewayTimeout);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Gateway Timeout");
    CHECK(result.code.value() == 504);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":504,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Gateway Timeout\"}");

    result.code = smlp::make_error_code(ErrorCode::HTTPVersionNotSupported);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "HTTP Version Not Supported");
    CHECK(result.code.value() == 505);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":505,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"HTTP Version Not Supported\"}");

    result.code =
        smlp::make_error_code(ErrorCode::NetworkAuthenticationRequired);
    result.action = "";
    result.data = R"({"test":"ok"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "Network Authentication Required");
    CHECK(result.code.value() == 511);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":511,\"data\":\"{\\\"test\\\":\\\"ok\\\"}"
          "\",\"message\":\"Network Authentication Required\"}");

    result.code = std::make_error_code(std::errc::file_exists);
    result.action = "";
    result.data = R"({"test":"file exists"})";
    CHECK(result.isSuccess() == false);
    CHECK(result.message() == "File exists");
    CHECK(result.code.value() == 17);
    CHECK(result.json() ==
          "{\"action\":\"\",\"code\":17,\"data\":\"{\\\"test\\\":\\\"file "
          "exists\\\"}\",\"message\":\"File exists\"}");
  }
}