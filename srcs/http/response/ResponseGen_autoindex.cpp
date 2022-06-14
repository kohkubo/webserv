#include "http/response/ResponseGenerator.hpp"

#include <dirent.h>
#include <sstream>

#include "utils/syscall_wrapper.hpp"

// TODO: DT_DIR系のマクロが定義されてない場合を考慮すべきかわからない
// TODO: DIR(ディレ), REG(通常ファイル), LNK(リンク)以外はどうするか
static std::string dir_list_lines(const std::string &file_path) {
  std::string    lines;
  DIR           *dir    = xopendir(file_path.c_str());
  struct dirent *diread = NULL;
  while ((diread = xreaddir(dir)) != NULL) {
    std::string name = diread->d_name;
    if (name == ".")
      continue;
    // ブラウザ上でlist表示された時,
    // ディレクトリが選択された場合にディレクトリとして再度リクエストが出されるために末尾に"/"
    // nginxでもディレクトリは末尾に"/"ついて表示されていた
    if ((diread->d_type & DT_DIR) != 0)
      name += "/";
    else if (!((diread->d_type & DT_REG) != 0 ||
               (diread->d_type & DT_LNK) != 0))
      continue;
    lines += "        <li><a href=\"" + name + "\">" + name + " </a></li>\n";
  }
  xclosedir(dir);
  return lines;
}

std::string
ResponseGenerator::__create_autoindex_body(const std::string &file_path,
                                           const RequestInfo &request_info) {
  std::stringstream buff;
  // clang-format off
  buff << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "   <head>\n"
       << "      <title>Index of " << request_info.request_target_ << "</title>\n"
       << "   </head>\n"
       << "   <body>\n"
       << "      <h1>Index of " << request_info.request_target_ << "</h1>\n"
       << "      <ul style=\"list-style:none\">\n"
       <<          dir_list_lines(file_path)
       << "    </ul>\n"
       << "   </body>\n"
       << "</html>";
  // clang-format on
  return buff.str();
}
