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
#include <cstdlib>
#include <optional>
#include <system_error>

namespace Common {
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
  RequestTimeout = 408,
  Conflict = 409,
  InternalServerError = 500,
  NotImplemented = 501,
  ServiceUnavailable = 503,
};

/**
 * @brief Common results
 * Can be use with Common::ErrorCode, std::io_errc, std::future_errc and
 std::errc
 * https://en.cppreference.com/w/cpp/error/errc
 * @code {.cpp}
  std::error_code code = std::make_error_code(std::io_errc::stream);
  std::error_code code = std::make_error_code(std::errc::permission_denied);
  std::error_code code =
 Common::make_error_code(Common::ErrorCode::Unauthorized); code.message();
  @endcode
 */
struct Result {
  std::error_code code;
  std::optional<std::string> message = std::nullopt;
  std::optional<std::string> error = std::nullopt;

  bool isSuccess() const {
    return code.value() == static_cast<int>(ErrorCode::Success) ||
           code.value() == static_cast<int>(ErrorCode::OK);
  }
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
    case ErrorCode::Forbidden:
      return "Forbidden";
    case ErrorCode::NotFound:
      return "Not Found";
    case ErrorCode::MethodNotAllowed:
      return "Method Not Allowed";
    case ErrorCode::NotAcceptable:
      return "Not Acceptable";
    case ErrorCode::RequestTimeout:
      return "Request Timeout";
    case ErrorCode::InternalServerError:
      return "Internal Server Error";
    case ErrorCode::NotImplemented:
      return "Not Implemented";
    case ErrorCode::ServiceUnavailable:
      return "Service Unavailable";
    default:
      return "Unknown error";
    }
  }
};

inline std::error_code make_error_code(ErrorCode e) {
  static ErrorCategory category;
  return {static_cast<int>(e), category};
}

} // namespace Common

namespace std {
template <> struct is_error_code_enum<Common::ErrorCode> : true_type {};
} // namespace std
