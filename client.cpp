#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

int main() {
  int         port = 55000;
  std::string ip   = "127.0.0.1";
  int         sock = 0;

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    exit(1);

  struct sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip.c_str());
  addr.sin_port        = htons(port);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    exit(1);
  sleep(1);

  std::string req = "GET / HTTP/1.1\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Host: 0.0.0.0:999\r\n"
                    "Accept: */*\r\n"
                    "\r\n"
                    "GET / HTTP/1.1\r\n"
                    "User-Agent: curl/7.68.0\r\n"
                    "Host: 0.0.0.0:999\r\n"
                    "Connection: Close\r\n"
                    "Accept: */*\r\n\r\n";
  write(sock, req.c_str(), req.size());

  // int send_count = 100;
  // std::cout << "send_count: " << send_count << std::endl;
  // std::vector<char> str(50000, 'A');
  // write(sock, &str[0], send_count);
  // sleep(1);
  char buf[5000];
  int  rc = read(sock, &buf, 5000);
  buf[rc] = '\0';
  std::cout << "rc: " << rc << std::endl;
  std::cout << buf << std::endl;
  close(sock);
  return 0;
}