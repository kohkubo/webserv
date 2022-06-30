#include "http/HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode(StatusCode status_code)
    : status_code_(status_code) {}

HttpStatusCode &HttpStatusCode::operator=(StatusCode status_code) {
  status_code_ = status_code;
  return *this;
}

HttpStatusCode &HttpStatusCode::operator=(const HttpStatusCode &other) {
  if (this == &other) {
    return *this;
  }
  status_code_ = other.status_code_;
  return *this;
}

HttpStatusCode &HttpStatusCode::operator=(const int num) {
  status_code_ = static_cast<StatusCode>(num);
  return *this;
}

bool HttpStatusCode::is_error_status_code() const {
  return 399 < status_code_ && status_code_ < 600;
}
bool HttpStatusCode::has_default_content() const {
  return 299 < status_code_ && status_code_ < 600;
}

bool operator==(const HttpStatusCode &lhs, const HttpStatusCode &rhs) {
  return lhs.status_code_ == rhs.status_code_;
}

HttpStatusCode::operator StatusCode() const { return status_code_; }
