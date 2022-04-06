#include <iostream>
#include "Webserv.hpp"

int main(int argc, char **argv) {
  (void)argv;
  if (argc != 2) {
    std::cerr << "Usage: ./webserv <config file>" << std::endl;
    return (1);
  }
  return (0);
}