#include "Webserv.hpp"
#include <iostream>

int main(int argc, char **argv) {
  (void)argv;
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <config file>" << std::endl;
    return (1);
  }
  server();
  return (0);
}
