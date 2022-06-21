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
  typedef std::string       name;
  typedef std::vector<name> nameVector;
  name                      updir_name_; // "../"のこと
  nameVector                dir_names_;
  nameVector                file_names_;
};

// TODO: DIR(ディレ), REG(通常ファイル), LNK(リンク)以外は無視している
//       検証方法を調べらてない+そもそも考えなくて良いと思っている rakiyama
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
      autoindex_category.dir_names_.push_back(name + "/");
    else
      autoindex_category.file_names_.push_back(name);
  }
  xclosedir(dir);
  return autoindex_category;
}

static std::string one_dir_list_line(std::string name) {
  return "        <li><a href=\"" + name + "\">" + name + " </a></li>\n";
}

static std::string read_vector_to_lines(AutoindexCategory::nameVector &names) {
  std::string res;
  std::sort(names.begin(), names.end());
  AutoindexCategory::nameVector::const_iterator it = names.begin();
  for (; it != names.end(); it++) {
    res += one_dir_list_line(*it);
  }
  return res;
}

static std::string dir_list_lines(const std::string &path) {
  AutoindexCategory autoindex_category = read_dir_to_autoindex_category(path);
  std::string       lines;
  lines += one_dir_list_line(autoindex_category.updir_name_);
  lines += read_vector_to_lines(autoindex_category.dir_names_);
  lines += read_vector_to_lines(autoindex_category.file_names_);
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
