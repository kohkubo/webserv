#ifndef INCLUDES_CONFIG_HPP_
#define INCLUDES_CONFIG_HPP_

#include <map>
#include <string>
#include <vector>

class Config {
public:
  typedef typename std::vector<std::string>           index;
  typedef typename std::string                        root;
  typedef typename std::map<std::string, std::string> listen;
  typedef typename std::string                        server_name;
  typedef typename std::map<std::string, std::string> return_;

  Config(const char *config_token);
  ~Config();
  root root_;

private:
  Config();
  Config(const Config &);
  Config     &operator=(const Config &);

  const char *__config_file_;
};

#endif // INCLUDES_CONFIG_HPP_
