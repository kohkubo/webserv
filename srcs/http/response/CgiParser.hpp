#ifndef SRCS_HTTP_RESPONSE_CGIPARSER
#define SRCS_HTTP_RESPONSE_CGIPARSER

#include <map>
#include <string>

#include "utils/Result.hpp"

// clang-format off
/*
rfc 3875 6. CGI Response
  CGI-Response = document-response | local-redir-response | client-redir-response | client-redirdoc-response

  document-response = Content-Type [ Status ] *other-field NL response-body
  local-redir-response = local-Location NL
  client-redir-response = client-Location *extension-field NL
  client-redirdoc-response = client-Location Status Content-Type *other-field NL response-body

    Content-Type = "Content-Type:" media-type NL

    Location        = local-Location | client-Location
    client-Location = "Location:" fragment-URI NL
    local-Location  = "Location:" local-pathquery NL

    fragment-URI    = absoluteURI [ "#" fragment ]
    local-pathquery = abs-path [ "?" query-string ]

    fragment        = *uric
    abs-path        = "/" path-segments
    path-segments   = segment *( "/" segment )
    segment         = *pchar
    pchar           = unreserved | escaped | extra
    extra           = ":" | "@" | "&" | "=" | "+" | "$" | ","

    Status         = "Status:" status-code SP reason-phrase NL
    status-code    = "200" | "302" | "400" | "501" | extension-code
    extension-code = 3digit
    reason-phrase  = *TEXT

    NL = LF or CRLF
 */
// clang-format on

/*
document-response
Content-Type: text/html
Status: 200
~~~
~~~
NL
hogehoge
 */

class CgiParser {
public:
  typedef std::map<std::string, std::string> HeaderMap;
  enum CgiState {
    HEADER,
    BODY,
    END,
    ERROR,
  };
  enum ResponseType {
    document,
    local_redir,
    client_redir,
    client_redirdoc,
  };
  CgiState     state_;
  ResponseType response_type_;
  std::string  new_line_delimiter_;
  std::size_t  content_length_;
  std::string  content_type_;
  std::string  content_;
  std::string  buffer_;
  HeaderMap    header_map_;

public:
  CgiParser()
      : state_(HEADER)
      , response_type_(document)
      , new_line_delimiter_("\n")
      , content_length_(0) {}
  ~CgiParser(){};
  CgiState handle_cgi(std::string &buffer);
  CgiState parse_header(std::string &buffer);
  CgiState parse_body(std::string &buffer);
};

#endif /* SRCS_HTTP_RESPONSE_CGIPARSER */
