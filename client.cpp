#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }
  if (argc != 4) {
    std::cerr << "ERROR: Not enough arguments!" << std::endl;
    return 1;
  }

  char* serverHost = argv[1];
  /*if (serverHost != "127.0.0.1") {
    std::cerr << "ERROR: Host must be 127.0.0.1 or localhost!" << std::endl;
    return 1;
  }*/

  int outPort = atoi(argv[2]); // convert cmd line arg to int
  if (outPort <= 1023) {
    std::cerr << "ERROR: Ports 0-1023 are reserved!" << std::endl;
    return 1;
  }

  char* fileName = argv[3];
  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(outPort);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr(serverHost);
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("connect");
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
    return 3;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;


  // send/receive data to/from connection
  bool isEnd = false;
  std::string input;
  char buf[100] = {0};
  std::stringstream ss;

  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

    std::cout << "send: ";
    std::cin >> input;
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }


    if (recv(sockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }
    ss << buf << std::endl;
    std::cout << "echo: ";
    std::cout << buf << std::endl;

    if (ss.str() == "close\n")
      break;

    ss.str("");
  }

  /*FILE *fd = fopen(fileName, "rb");
  fclose(fd);*/
  std::cout << "Socket closed\n";

  close(sockfd);

  return 0;
}