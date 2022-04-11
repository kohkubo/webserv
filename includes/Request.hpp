#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>

class Request {
public:
  Request(const std::string &message);
  ~Request() {}
  std::string get_method() const { return __method_; }
  std::string get_uri() const { return __uri_; }
  Request(Request const &other);
  Request &operator=(Request const &other);

private:
  Request() {}
  std::string              __method_;
  std::string              __uri_;
  std::string              __version_;
  std::vector<std::string> __fields_;
};

#endif /* REQUEST_HPP */
