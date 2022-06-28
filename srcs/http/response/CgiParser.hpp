#ifndef SRCS_HTTP_RESPONSE_CGIPARSER
#define SRCS_HTTP_RESPONSE_CGIPARSER

#include <string>

// clang-format off
/*
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

class CgiParser {
private:
  enum ResponseType {
    document,
    local_redir,
    client_redir,
    client_redirdoc,
  };

public:
  CgiParser();
  CgiParser(const std::string &cgi_response);
  ~CgiParser();
};

#endif /* SRCS_HTTP_RESPONSE_CGIPARSER */
