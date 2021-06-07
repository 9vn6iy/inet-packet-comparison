#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
/*
 * close()
 * */
#include <unistd.h>
/*
 * socket()
 * */
#include <sys/socket.h>
/*
 * struct addrinfo
 * getaddrinfo()
 * gai_strerror()
 * freeaddrinfo()
 * */
#include <netdb.h>          

#include "../../include/HandleError.h"
#include "../../include/constant.h"

int main(int argc, char const *argv[]) {
  char servIP[] = "127.0.0.1";
  char servPort[] = "5201";

  // construct the address criteria
  /*
  struct addrinfo addrCriteria;
  addrCriteria.ai_family = AF_UNSPEC;
  addrCriteria.ai_socktype = SOCK_DGRAM;
  addrCriteria.ai_protocol = IPPROTO_UDP;
  // addrCriteria.ai_flags = 0;
  */


  // construct the data
  const char data[] = "client sending data to server...";
  size_t dataLen = strlen(data);

  while(1) {
    // get server address
    /*
    struct addrinfo *servAddr;
    int rtn = getaddrinfo(servIP, servPort, &addrCriteria, &servAddr);
    if (rtn != 0) {
      DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtn));
    }
    */
    

    // construct client socket
    // int clntSock = socket(servAddr->ai_family, servAddr->ai_socktype, 
      //servAddr->ai_protocol);
    int clntSock = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    memset(&ifr, 0x00, sizeof(ifr));
    strncpy(ifr.ifr_name, "ens33", IFNAMSIZ);
    // int protoNumber = getprotobyname("UDP");
    int opt = setsockopt(clntSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    if (opt < 0) {
      DieWithSystemMessage("setsockopt() failed");
    }
    if (clntSock < 0) {
      DieWithSystemMessage("socket() failed");
    }

    // send data to the server
    ssize_t numBytes = sendto(clntSock, data, dataLen, 0, 
        servIP, sizeof(servIP));
    // ssize_t numBytes = sendto(clntSock, data, dataLen, 0, 
      // servAddr->ai_addr, servAddr->ai_addrlen);
    if (numBytes < 0) {
      DieWithSystemMessage("sendto() failed");
    } else if (numBytes != dataLen) {
      DieWithUserMessage("sendto() error", "sent unexpected number of bytes");
    }

    // receive a response
    struct sockaddr_storage fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
    char buffer[MAX_STRING_LENGTH + 1];
    numBytes = recvfrom(clntSock, buffer, MAX_STRING_LENGTH, 0, 
        (struct sockaddr *)&fromAddr, &fromAddrLen);
    if (numBytes < 0) {
      DieWithUserMessage("recvfrom() error", "received unexpected number of bytes");
    }
    // freeaddrinfo(servAddr);
    buffer[dataLen] = '\0';
    printf("Received: %s\n", buffer);
    close(clntSock);
  }
  exit(0);
}


