#include "http/HttpStatusCode.hpp"

HttpStatusCode::HttpStatusCode(StatusCode status_code)
    : _status_code_(status_code) {}

HttpStatusCode &HttpStatusCode::operator=(StatusCode status_code) {
  _status_code_ = status_code;
  return *this;
}

HttpStatusCode &HttpStatusCode::operator=(const HttpStatusCode &other) {
  if (this == &other) {
    return *this;
  }
  _status_code_ = other.status_code();
  return *this;
}

HttpStatusCode &HttpStatusCode::operator=(const int num) {
  _status_code_ = static_cast<StatusCode>(num);
  return *this;
}

bool HttpStatusCode::is_error_status_code() const {
  return 399 < _status_code_ && _status_code_ < 600;
}
bool HttpStatusCode::has_default_content() const {
  return 299 < _status_code_ && _status_code_ < 600;
}

HttpStatusCode::StatusCode HttpStatusCode::status_code() const {
  return _status_code_;
}

void HttpStatusCode::set_status(StatusCode status_code) {
  _status_code_ = status_code;
}

bool operator==(const HttpStatusCode &lhs, const HttpStatusCode &rhs) {
  return lhs.status_code() == rhs.status_code();
}

HttpStatusCode::operator StatusCode() const { return _status_code_; }
