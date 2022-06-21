#include "http/response/ResponseGenerator.hpp"

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <vector>

#include "utils/syscall_wrapper.hpp"

/* autoindex example
../
dir1/
dir2/
file1/
file2/
*/
struct AutoindexCategory {
  typedef std::string       name;
  typedef std::vector<name> nameVector;
  name                      updir_name_;
  nameVector                dir_name_vector_;
  nameVector                file_name_vector_;
};

static std::string dir_listing_line(std::string name) {
  if (name == "") {
    return "";
  }
  return "        <li><a href=\"" + name + "\">" + name + " </a></li>\n";
}

// TODO: DT_DIR系のマクロが定義されてない場合を考慮すべきかわからない
// TODO: DIR(ディレ), REG(通常ファイル), LNK(リンク)以外はどうするか
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
      autoindex_category.updir_name_ = name + "/";
    else if ((diread->d_type & DT_DIR) != 0)
      autoindex_category.dir_name_vector_.push_back(name + "/");
    else
      autoindex_category.file_name_vector_.push_back(name);
  }
  xclosedir(dir);
  return autoindex_category;
}

static std::string
read_vector_to_line(AutoindexCategory::nameVector &name_vector) {
  std::string res;
  std::sort(name_vector.begin(), name_vector.end());
  AutoindexCategory::nameVector::const_iterator it = name_vector.begin();
  for (; it != name_vector.end(); it++) {
    res += dir_listing_line(*it);
  }
  return res;
}

static std::string dir_list_lines(const std::string &path) {
  AutoindexCategory autoindex_category = read_dir_to_autoindex_category(path);
  std::string       lines;
  lines += dir_listing_line(autoindex_category.updir_name_);
  lines += read_vector_to_line(autoindex_category.dir_name_vector_);
  lines += read_vector_to_line(autoindex_category.file_name_vector_);
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
       <<          dir_list_lines(request_info.file_path_)
       << "    </ul>\n"
       << "   </body>\n"
       << "</html>";
  // clang-format on
  return buff.str();
}
