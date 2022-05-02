#ifndef INCLUDES_SERVERCONFIG_HPP
#define INCLUDES_SERVERCONFIG_HPP

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

class ServerConfig {
public:
  std::string listen_address_;
  std::string listen_port_;
  int         client_max_body_size_;
  std::string server_name_;
  std::string root_;
  std::string index_;

  // error_page;
public:
  class UnexpectedTokenException : public std::logic_error {
  public:
    UnexpectedTokenException(const std::string &msg = "unexpected token.");
  };

public:
  ServerConfig();
  ~ServerConfig(){};
  std::vector<std::string>::iterator
  parse(std::vector<std::string>::iterator pos,
        std::vector<std::string>::iterator end);

private:
  std::vector<std::string>::iterator
  __parse_listen(std::vector<std::string>::iterator pos,
                 std::vector<std::string>::iterator end);
  std::vector<std::string>::iterator
  __parse_root(std::vector<std::string>::iterator pos,
               std::vector<std::string>::iterator end);
  std::vector<std::string>::iterator
  __parse_server_name(std::vector<std::string>::iterator pos,
                      std::vector<std::string>::iterator end);
};

typedef std::vector<std::vector<const ServerConfig *> >   server_group_type;
typedef std::map<int, std::vector<const ServerConfig *> > socket_list_type;

server_group_type
create_server_group(const std::vector<ServerConfig> &server_list);

#endif /* INCLUDES_SERVERCONFIG_HPP */
