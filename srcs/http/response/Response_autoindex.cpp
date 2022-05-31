#include "http/response/Response.hpp"

#include <dirent.h>
#include <sstream>

#include "utils/syscall_wrapper.hpp"

static std::string dir_list_lines(const std::string &file_path) {
  std::string    lines;
  DIR           *dir = xopendir(file_path.c_str());
  struct dirent *diread;
  while ((diread = readdir(dir)) != NULL) {
    std::string file_name = diread->d_name;
    // clang-format off
    lines += "        <li><a href=\"" + file_name + "\">" + file_name + " </a></li>\n";
    // clang-format on
  }
  closedir(dir);
  return lines;
}

std::string Response::__create_autoindex_body(const std::string &file_path) {
  std::stringstream buff;
  // clang-format off
  buff << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "   <head>\n"
       << "      <title>Index of " << __request_info_.uri_ << "</title>\n"
       << "   </head>\n"
       << "   <body>\n"
       << "      <h1>Index of " << __request_info_.uri_ << "</h1>\n"
       << "      <ul style=\"list-style:none\">\n"
       <<          dir_list_lines(file_path)
       << "    </ul>\n"
       << "   </body>\n"
       << "</html>";
  // clang-format on
  return buff.str();
}
