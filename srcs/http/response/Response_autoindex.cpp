#include "http/response/Response.hpp"

#include <dirent.h>
#include <sstream>

#include "utils/syscall_wrapper.hpp"

// TODO: DT_DIR系のマクロが定義されてない場合を考慮すべきかわからない
// TODO: DIR(ディレ), REG(通常ファイル), LNK(リンク)以外はどうするか
static std::string dir_list_lines(const std::string &file_path) {
  std::string    lines;
  DIR           *dir = xopendir(file_path.c_str());
  struct dirent *diread;
  while ((diread = readdir(dir)) != NULL) {
    std::string name = diread->d_name;
    if (name == ".")
      continue;
    if (diread->d_type & DT_DIR)
      name += "/";
    else if (!((diread->d_type & DT_REG) || (diread->d_type & DT_LNK)))
      continue;
    lines += "        <li><a href=\"" + name + "\">" + name + " </a></li>\n";
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
