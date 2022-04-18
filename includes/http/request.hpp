#ifndef INCLUDES_HTTP_REQUEST_HPP
#define INCLUDES_HTTP_REQUEST_HPP

#include "Lexer.hpp"
#include "http/define.hpp"

message_type parse_request(Lexer &request_lexer);

#endif /* INCLUDES_HTTP_REQUEST_HPP */
