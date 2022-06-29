#include <dirent.h>

#include <algorithm>
#include <set>
#include <sstream>

#include "http/request/RequestInfo.hpp"
#include "utils/syscall_wrapper.hpp"

struct AutoindexCategory {
  typedef std::string         entryName;
  typedef std::set<entryName> entryNames;
  bool                        has_updir_;
  entryNames                  dirs_;
  entryNames                  files_;
};

// Reference of acceptable entry types:
// nginx/ngx_http_autoindex_module.c at master · nginx/nginx
// https://github.com/nginx/nginx/blob/master/src/http/modules/ngx_http_autoindex_module.c
static AutoindexCategory read_dir_to_entry_category(const std::string &path) {
  AutoindexCategory autoindex_category;
  DIR              *dir   = xopendir(path.c_str());
  struct dirent    *entry = NULL;
  while ((entry = xreaddir(dir)) != NULL) {
    AutoindexCategory::entryName entry_name = entry->d_name;
    bool                         is_dir     = (entry->d_type & DT_DIR) != 0;
    bool                         is_file    = (entry->d_type & DT_REG) != 0;
    bool                         is_link    = (entry->d_type & DT_LNK) != 0;
    if (entry_name == "." || !(is_dir || is_file || is_link))
      continue;
    if (entry_name == "..")
      autoindex_category.has_updir_ = true;
    else if (is_dir)
      autoindex_category.dirs_.insert(entry_name + "/");
    else
      autoindex_category.files_.insert(entry_name);
  }
  xclosedir(dir);
  return autoindex_category;
}

static std::string
one_dir_listing_line(const AutoindexCategory::entryName &entry_name) {
  // clang-format off
  return "        <li><a href=\"" + entry_name + "\">" + entry_name + " </a></li>\n";
  // clang-format on
}

static std::string read_entry_names_to_dir_listing_lines(
    AutoindexCategory::entryNames &entry_names) {
  std::string                                   lines;
  AutoindexCategory::entryNames::const_iterator it = entry_names.begin();
  for (; it != entry_names.end(); it++) {
    lines += one_dir_listing_line(*it);
  }
  return lines;
}

static std::string dir_listing_lines(const std::string &path) {
  AutoindexCategory autoindex_category = read_dir_to_entry_category(path);
  std::string       lines;
  if (autoindex_category.has_updir_)
    lines += one_dir_listing_line("../");
  lines += read_entry_names_to_dir_listing_lines(autoindex_category.dirs_);
  lines += read_entry_names_to_dir_listing_lines(autoindex_category.files_);
  return lines;
}

std::string create_autoindex_body(const RequestInfo &request_info,
                                  const std::string &target_path) {
  // LOG("create autoindex body");
  std::stringstream buff;
  // clang-format off
  buff << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "   <head>\n"
       << "      <title>Index of " << request_info.request_line_.absolute_path_ << "</title>\n"
       << "   </head>\n"
       << "   <body>\n"
       << "      <h1>Index of " << request_info.request_line_.absolute_path_ << "</h1>\n"
       << "      <ul style=\"list-style:none\">\n"
       <<          dir_listing_lines(target_path)
       << "    </ul>\n"
       << "   </body>\n"
       << "</html>";
  // clang-format on
  return buff.str();
}
