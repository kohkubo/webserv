#ifndef SRCS_UTILS_UTILS_HPP
#define SRCS_UTILS_UTILS_HPP

#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "utils/Result.hpp"

#define RESET          "\033[0m"
#define BOLD           "\033[1m"
#define BLACK          "\033[30m"
#define RED            "\033[31m"
#define GREEN          "\033[32m"
#define YELLOW         "\033[33m"
#define BLUE           "\033[34m"
#define MAGENTA        "\033[35m"
#define CYAN           "\033[36m"
#define WHITE          "\033[37m"
#define UNDERLINE      "\033[4m"
#define BOLD_UNDERLINE "\033[1;4m"

#define ERROR_LOG(msg)                                                         \
  do {                                                                         \
    std::cerr << MAGENTA "[ERROR_LOG ] " << RESET << msg << std::endl;         \
  } while (0)

#define ERROR_LOG_WITH_ERRNO(msg)                                              \
  do {                                                                         \
    std::cerr << MAGENTA "[ERROR_LOG ] " << RESET << msg << ": "               \
              << strerror(errno) << std::endl;                                 \
  } while (0)

#define LOG(msg)                                                               \
  do {                                                                         \
    std::cout << BLUE "[ LOG      ] " << RESET << msg << std::endl;            \
  } while (0)

#define ERROR_EXIT(msg)                                                        \
  do {                                                                         \
    ERROR_LOG(msg);                                                            \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define ERROR_EXIT_WITH_ERRNO(msg)                                             \
  do {                                                                         \
    ERROR_LOG_WITH_ERRNO(msg);                                                 \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

bool        has_suffix(const std::string &str, const std::string &suffix);
bool        has_prefix(const std::string &str, const std::string &prefix);
std::string trim(std::string str, std::string cutset);
std::string tolower(const std::string &str);
bool        getline(std::string &source, std::string &line);

template <typename T>
std::string to_string(T val) {
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

Result<std::size_t> hexstr_to_size(const std::string &str);
bool                is_dir(const std::string &filepath);
bool                is_minus_depth(const std::string &filepath);

#endif /* SRCS_UTILS_UTILS_HPP */
