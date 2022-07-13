#ifndef SRCS_HTTP_CONTENTINFO_HPP
#define SRCS_HTTP_CONTENTINFO_HPP

#include <string>

class ContentInfo {
public:
  std::string content_type_;
  ssize_t     content_length_;
  std::string content_;

public:
  ContentInfo()
      : content_length_(-1) {}
  bool has_content_length() const { return content_length_ >= 0; }
};

#endif /* SRCS_HTTP_CONTENTINFO_HPP */
