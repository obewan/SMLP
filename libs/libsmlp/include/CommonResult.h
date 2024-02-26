/**
 * @file CommonResult.h
 * @author Damien Balima (www.dams-labs.net)
 * @brief Common Result
 * @date 2024-02-12
 *
 * @copyright Damien Balima (c) CC-BY-NC-SA-4.0 2024
 *
 */
#pragma once
#include "json.hpp"
#include <cstdlib>
#include <optional>
#include <system_error>

namespace smlp {
/**
 * @brief Common HTTP code and common Unix code
 * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
 *
 */
enum class ErrorCode {
  Success = 0,
  Failure = 1,
  OK = 200,
  Created = 201,
  Accepted = 202,
  BadRequest = 400,
  Unauthorized = 401,
  PaymentRequired = 402,
  Forbidden = 403,
  NotFound = 404,
  MethodNotAllowed = 405,
  NotAcceptable = 406,
  ProxyAuthenticationRequired = 407,
  RequestTimeout = 408,
  Conflict = 409,
  InternalServerError = 500,
  NotImplemented = 501,
  BadGateway = 502,
  ServiceUnavailable = 503,
  GatewayTimeout = 504,
  HTTPVersionNotSupported = 505,
  NetworkAuthenticationRequired = 511,
};

const std::map<ErrorCode, std::string> errorCodeMessages{
    {ErrorCode::Success, "Success"},
    {ErrorCode::Failure, "Failure"},
    {ErrorCode::OK, "OK"},
    {ErrorCode::Created, "Created"},
    {ErrorCode::Accepted, "Accepted"},
    {ErrorCode::BadRequest, "Bad Request"},
    {ErrorCode::Unauthorized, "Unauthorized"},
    {ErrorCode::PaymentRequired, "Payment Required"},
    {ErrorCode::Forbidden, "Forbidden"},
    {ErrorCode::NotFound, "Not Found"},
    {ErrorCode::MethodNotAllowed, "Method Not Allowed"},
    {ErrorCode::NotAcceptable, "Not Acceptable"},
    {ErrorCode::ProxyAuthenticationRequired, "Proxy Authentication Required"},
    {ErrorCode::RequestTimeout, "Request Timeout"},
    {ErrorCode::Conflict, "Conflict"},
    {ErrorCode::InternalServerError, "Internal Server Error"},
    {ErrorCode::NotImplemented, "Not Implemented"},
    {ErrorCode::BadGateway, "Bad Gateway"},
    {ErrorCode::ServiceUnavailable, "Service Unavailable"},
    {ErrorCode::GatewayTimeout, "Gateway Timeout"},
    {ErrorCode::HTTPVersionNotSupported, "HTTP Version Not Supported"},
    {ErrorCode::NetworkAuthenticationRequired,
     "Network Authentication Required"}};

class ErrorCategory : public std::error_category {
public:
  const char *name() const noexcept override { return "server"; }

  std::string message(int ev) const override {
    auto it = errorCodeMessages.find(static_cast<ErrorCode>(ev));
    if (it != errorCodeMessages.end()) {
      return it->second;
    }
    return "Unknown Error";
  }
};

inline std::error_code make_error_code(ErrorCode e) {
  static ErrorCategory category;
  return {static_cast<int>(e), category};
}

/**
 * @brief Common results
 * Can be use with smlp::ErrorCode, std::io_errc, std::future_errc and
 std::errc
 * https://en.cppreference.com/w/cpp/error/errc
 * @code {.cpp}
  std::error_code code = std::make_error_code(std::io_errc::stream);
  std::error_code code = std::make_error_code(std::errc::permission_denied);
  std::error_code code =
 smlp::make_error_code(smlp::ErrorCode::Unauthorized); code.message();
  @endcode
 */
struct Result {
  std::error_code code = smlp::make_error_code(smlp::ErrorCode::Failure);

  std::optional<std::string> action = std::nullopt;
  std::optional<std::string> technical = std::nullopt;
  std::optional<std::string> data = std::nullopt;

  bool isSuccess() const {
    return code.value() == static_cast<int>(ErrorCode::Success) ||
           (code.value() >= 200 && code.value() < 300);
  }

  std::string message() const { return technical.value_or(code.message()); }

  std::string json() const {
    nlohmann::json jdata;
    jdata["code"] = code.value();
    jdata["message"] = message();
    jdata["action"] = action.value_or("");
    jdata["data"] = data.value_or("");
    return jdata.dump();
  }
};

} // namespace smlp

namespace std {
template <> struct is_error_code_enum<smlp::ErrorCode> : true_type {};
} // namespace std
