#ifndef SRCS_HTTP_RESPONSE_CGIPARSER
#define SRCS_HTTP_RESPONSE_CGIPARSER

#include <map>
#include <string>

#include "http/HeaderFieldMap.hpp"
#include "http/HttpStatusCode.hpp"
#include "utils/Result.hpp"

// clang-format off
/*
rfc 3875 6. CGI Response
  CGI-Response = document-response | local-redir-response | client-redir-response | client-redirdoc-response

  document-response = Content-Type "Content-Type:" media-type NL
                      [ Status ]   "Status:" status-code SP reason-phrase NL
                      *other-field
                      NL
                      response-body
  local-redir-response = local-Location  "Location:" local-pathquery NL
                         NL
  client-redir-response = client-Location "Location:" local-pathquery NL
                          *extension-field
                          NL
  client-redirdoc-response = client-Location
                             Status
                             Content-Type
                             *other-field
                             NL
                             response-body

    Content-Type = "Content-Type:" media-type NL

    Location        = local-Location | client-Location
    client-Location = "Location:" fragment-URI NL         // /ではじまる文字列
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
  enum CgiState {
    HEADER,
    HEADER_END,
    BODY,
    END,
    ERROR,
  };
  enum ResponseType {
    DOCUMENT,
    LOCAL_REDIR,
    CLIENT_REDIR,
    CLIENT_REDIRDOC,
  };
  struct CgiLocation {
    std::string path_;
    std::string query_;
  };
  CgiState       state_;
  ResponseType   response_type_;
  CgiLocation    cgi_location_;
  std::size_t    content_length_;
  bool           has_content_length_;
  std::string    content_type_;
  std::string    content_;
  std::string    buffer_;
  HeaderFieldMap header_field_map_;
  HttpStatusCode status_code_;

public:
  CgiParser()
      : state_(HEADER)
      , response_type_(DOCUMENT)
      , content_length_(0)
      , has_content_length_(false) {}
  ~CgiParser(){};
  CgiState handle_cgi(std::string &buffer);
  CgiState parse_header(std::string &buffer);
  CgiState parse_header_end();
  CgiState parse_body(std::string &buffer);
};

#endif /* SRCS_HTTP_RESPONSE_CGIPARSER */
