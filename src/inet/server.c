#include <stdio.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "../../include/constant.h"
#include "../../include/HandleError.h"
#include "../../include/AddressUtility.h"

int main(int argc, char const *argv[]) {
  const char servPort[] = "5201";

  // construct server address structure
  /*
  struct addrinfo addrCriteria;
  addrCriteria.ai_family = AF_UNSPEC;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_DGRAM;
  addrCriteria.ai_protocol = IPPROTO_UDP;


  struct addrinfo *servAddr;
  int rtn = getaddrinfo(NULL, servPort, &addrCriteria, &servAddr);
  if (rtn != 0) {
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtn));
  }
  */

  // constructe socketA
  int servSock = socket(AF_INET, SOCK_DGRAM, 0);
  // int servSock = socket(servAddr->ai_family, servAddr->ai_socktype, 
     // servAddr->ai_protocol);
  /*
  struct ifreq ifr;
  memset(&ifr, 0x00, sizeof(ifr));
  strncpy(ifr.ifr_name, "ens33", IFNAMSIZ);
  // int protoNumber = getprotobyname("UDP");
  int opt = setsockopt(servSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
  if (opt < 0) {
    DieWithSystemMessage("setsockopt() failed");
  }
  */
  if (servSock < 0) {
    DieWithSystemMessage("socket() failed");
  }

  // bind to the local address
  if (bind(servSock, servPort, sizeof(servPort)) < 0) {
  // if (bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) < 0) {
    DieWithSystemMessage("bind() failed");
  }
  // freeaddrinfo(servAddr);

  while (1) {
    puts("start listening...");
    struct sockaddr_storage clntAddr;
    socklen_t clntAddrLen = sizeof(clntAddr);
    char buffer[MAX_STRING_LENGTH];
    ssize_t numBytesRcvd = recvfrom(servSock, buffer, MAX_STRING_LENGTH, 0, 
        (struct sockaddr *)&clntAddr, &clntAddrLen);
    if (numBytesRcvd < 0) {
      DieWithSystemMessage("recvfrom() failed");
    }
    fputs("Handling client ", stdout);
    PrintSocketAddress((struct sockaddr *)&clntAddr, stdout);
    fputc('\n', stdout);

    // send recv datagram back to the client
    ssize_t numBytesSent = sendto(servSock, buffer, numBytesRcvd, 0,
        (struct sockaddr *)&clntAddr, sizeof(clntAddr));
    if (numBytesSent < 0) {
      DieWithSystemMessage("sendto() failed");
    } else if (numBytesSent != numBytesRcvd) {
      DieWithUserMessage("sendto()", "sent unexpected number of bytes");
    }
  }
  close(servSock);
  exit(0);
}


