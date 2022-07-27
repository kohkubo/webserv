#ifndef SRCS_HTTP_RESPONSE_CGIINFO_HPP
#define SRCS_HTTP_RESPONSE_CGIINFO_HPP

#include <map>
#include <string>

#include "http/ContentInfo.hpp"
#include "http/HeaderFieldMap.hpp"
#include "http/HttpStatusCode.hpp"
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

class CgiInfo {
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
  HeaderFieldMap header_field_map_;

  ContentInfo content_info_;
  std::string content_;

  std::string http_status_phrase_;
  std::size_t status_digit_;

  static const std::size_t BUFFER_MAX_LENGTH_  = 8192;
  static const std::size_t CONTENT_MAX_LENGTH_ = 8192;

private:
  bool _is_valid_header() const;
  bool _parse_status_header();

public:
  CgiInfo()
      : state_(HEADER)
      , response_type_(DOCUMENT)
      , status_digit_(0) {}
  ~CgiInfo(){};
  CgiState    handle_cgi(std::string &buffer);
  CgiState    parse_header(std::string &buffer);
  CgiState    parse_header_end();
  CgiState    parse_body(std::string &buffer);
  std::string http_start_line() const;
};

#endif /* SRCS_HTTP_RESPONSE_CGIINFO_HPP */
