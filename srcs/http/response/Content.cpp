#include "http/response/Content.hpp"

namespace response_generator {

ResponseInfo::ResponseInfo(HttpStatusCode status_code)
    : action_(READ)
    , status_code_(status_code)
    , fd_(-1)
    , cgi_pid_(-1) {}

// created
ResponseInfo::ResponseInfo(std::string content, HttpStatusCode status_code,
                           Action action)
    : action_(action)
    , status_code_(status_code)
    , content_(content)
    , fd_(-1) {}

ResponseInfo::ResponseInfo(Action action, HttpStatusCode status_code,
                           std::string content, int fd, pid_t pid)
    : action_(action)
    , status_code_(status_code)
    , content_(content)
    , fd_(fd)
    , cgi_pid_(pid) {}

WriteContent::WriteContent(int fd, std::string content)
    : ResponseInfo(ResponseInfo::WRITE, HttpStatusCode::S_201_CREATED, content,
                   fd, -1) {}

ReadContent::ReadContent(int fd, HttpStatusCode status_code)
    : ResponseInfo(ResponseInfo::READ, status_code, std::string(), fd, -1) {}

// cgi
CgiContent::CgiContent(int fd, pid_t cgi_pid)
    : ResponseInfo(ResponseInfo::CGI, HttpStatusCode::S_200_OK, std::string(),
                   fd, cgi_pid) {}

} // namespace response_generator