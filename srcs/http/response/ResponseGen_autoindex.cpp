#include "http/response/ResponseGenerator.hpp"

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <vector>

#include "utils/syscall_wrapper.hpp"

/* listing order of autoindex
 * ../
 * dir1/
 * dir2/
 * file1/
 * file2/
 */
struct AutoindexCategory {
  typedef std::string        entry;
  typedef std::vector<entry> entrys;
  entry                      updir_;
  entrys                     dirs_;
  entrys                     files_;
};

// TODO: vector -> set
// file type(DT_DIR,DT_REG,DT_LNK):
// nginx/ngx_http_autoindex_module.c at master · nginx/nginx
// https://github.com/nginx/nginx/blob/master/src/http/modules/ngx_http_autoindex_module.c
static AutoindexCategory
read_dir_to_autoindex_category(const std::string &path) {
  AutoindexCategory autoindex_category;
  DIR              *dir    = xopendir(path.c_str());
  struct dirent    *diread = NULL;
  while ((diread = xreaddir(dir)) != NULL) {
    std::string name = diread->d_name;
    if (name == "." || (diread->d_type & (DT_DIR | DT_REG | DT_LNK)) == 0)
      continue;
    if (name == "..")
      autoindex_category.updir_ = "../";
    else if ((diread->d_type & DT_DIR) != 0)
      autoindex_category.dirs_.push_back(name + "/");
    else
      autoindex_category.files_.push_back(name);
  }
  xclosedir(dir);
  return autoindex_category;
}

static std::string
one_dirlisting_line(const AutoindexCategory::entry &file_name) {
  return "        <li><a href=\"" + file_name + "\">" + file_name +
         " </a></li>\n";
}

static std::string
read_vector_to_dirlisting_lines(AutoindexCategory::entrys &file_names) {
  std::string res;
  std::sort(file_names.begin(), file_names.end());
  AutoindexCategory::entrys::const_iterator it = file_names.begin();
  for (; it != file_names.end(); it++) {
    res += one_dirlisting_line(*it);
  }
  return res;
}

static std::string dirlisting_lines(const std::string &path) {
  AutoindexCategory autoindex_category = read_dir_to_autoindex_category(path);
  std::string       lines;
  if (autoindex_category.updir_ != "") {
    lines += one_dirlisting_line(autoindex_category.updir_);
  }
  lines += read_vector_to_dirlisting_lines(autoindex_category.dirs_);
  lines += read_vector_to_dirlisting_lines(autoindex_category.files_);
  return lines;
}

std::string
ResponseGenerator::_create_autoindex_body(const RequestInfo &request_info) {
  LOG("create autoindex body");
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
       <<          dirlisting_lines(request_info.file_path_)
       << "    </ul>\n"
       << "   </body>\n"
       << "</html>";
  // clang-format on
  return buff.str();
}
