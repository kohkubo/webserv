#ifndef SRCS_HTTP_CONST_CONST_HTTP_ENUMS_HPP
#define SRCS_HTTP_CONST_CONST_HTTP_ENUMS_HPP

namespace HttpStatusCode {
enum StatusCode {
  OK_200                    = 200,
  NO_CONTENT_204            = 204,
  MOVED_PERMANENTLY_301     = 301,
  NOT_MODIFIED_304          = 304,
  BAD_REQUEST_400           = 400,
  FORBIDDEN_403             = 403,
  NOT_FOUND_404             = 404,
  NOT_ALLOWED_405           = 405,
  ENTITY_TOO_LARGE_413      = 413,
  INTERNAL_SERVER_ERROR_500 = 500,
  NOT_IMPLEMENTED_501       = 501,
  UNKNOWN_ERROR_520         = 520,
};
}

#endif /* SRCS_HTTP_CONST_CONST_HTTP_ENUMS_HPP */
