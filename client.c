#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void error_message(int line);

int main() {
  int port = 5001;
  char *ip = "127.0.0.1";
  int sock;

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    error_message(__LINE__);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    error_message(__LINE__);

  while(1);
  printf("\n");
  close(sock);
  exit(1);
  return 1;
}

void error_message(int line) {
  printf("ERROR: LINE %d", line);
  exit(1);
}