#ifndef DEMO_HTTP1PARSER_HPP_
#define DEMO_HTTP1PARSER_HPP_

#include <string>
#include <cstring>
#include <unistd.h>
#include "configure.hpp"

class HTTP1_Parser {
public:
    static char *get_filename(const char *excutive_file);
    static std::string argv_path_analyzer(std::string request_path, const char *path, const char *executive_file);
    static std::string get_requestline_path(char _pbuf[BUF_SIZE]);
};

#endif  // DEMO_HTTP1PARSER_HPP_
