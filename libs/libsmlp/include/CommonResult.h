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

class ErrorCategory : public std::error_category {
public:
  const char *name() const noexcept override { return "server"; }

  std::string message(int ev) const override {
    switch (static_cast<ErrorCode>(ev)) {
    case ErrorCode::Success:
      return "Success";
    case ErrorCode::Failure:
      return "Failure";
    case ErrorCode::OK:
      return "OK";
    case ErrorCode::Created:
      return "Created";
    case ErrorCode::Accepted:
      return "Accepted";
    case ErrorCode::BadRequest:
      return "Bad Request";
    case ErrorCode::Unauthorized:
      return "Unauthorized";
    case ErrorCode::PaymentRequired:
      return "Payment Required";
    case ErrorCode::Forbidden:
      return "Forbidden";
    case ErrorCode::NotFound:
      return "Not Found";
    case ErrorCode::MethodNotAllowed:
      return "Method Not Allowed";
    case ErrorCode::NotAcceptable:
      return "Not Acceptable";
    case ErrorCode::ProxyAuthenticationRequired:
      return "Proxy Authentication Required";
    case ErrorCode::RequestTimeout:
      return "Request Timeout";
    case ErrorCode::Conflict:
      return "Conflict";
    case ErrorCode::InternalServerError:
      return "Internal Server Error";
    case ErrorCode::NotImplemented:
      return "Not Implemented";
    case ErrorCode::BadGateway:
      return "Bad Gateway";
    case ErrorCode::ServiceUnavailable:
      return "Service Unavailable";
    case ErrorCode::GatewayTimeout:
      return "Gateway Timeout";
    case ErrorCode::HTTPVersionNotSupported:
      return "HTTP Version Not Supported";
    case ErrorCode::NetworkAuthenticationRequired:
      return "Network Authentication Required";
    default:
      return "Unknown Error";
    }
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
