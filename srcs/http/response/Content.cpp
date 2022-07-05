#include "http/response/Content.hpp"

namespace response_generator {

Content::Content(HttpStatusCode status_code)
    : action_(READ)
    , status_code_(status_code)
    , fd_(-1)
    , cgi_pid_(-1) {}

// file read
Content::Content(int fd, Action action, HttpStatusCode status_code)
    : action_(action)
    , status_code_(status_code)
    , fd_(fd)
    , cgi_pid_(-1) {}

// cgi
Content::Content(int fd, Action action, pid_t cgi_pid)
    : action_(action)
    , status_code_(HttpStatusCode::S_200_OK)
    , fd_(fd)
    , cgi_pid_(cgi_pid) {}

// created
Content::Content(std::string content, HttpStatusCode status_code, Action action)
    : action_(action)
    , status_code_(status_code)
    , content_(content)
    , fd_(-1) {}

} // namespace response_generator