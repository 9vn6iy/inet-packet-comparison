#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>          
#include <arpa/inet.h>

#include "../../include/HandleError.h"
#include "../../include/constant.h"


int main(int argc, char const *argv[]) {
  char servIP[] = "10.252.152.130";
  char servPort[] = "5201";
  const char data[] = "client sending data to server...";
  size_t dataLen = strlen(data);
  char buffer[MAX_STRING_LENGTH + 1];
  while (1) {
    int clntSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (clntSock < 0) {
      DieWithSystemMessage("socket() failed");
    }
    // set network card
    struct ifreq ifr;
    memset(&ifr, 0x00, sizeof(ifr));
    char card[] = "ens33";
    // strncpy(ifr.ifr_name, "ens33", strlen("ens33"));
    strncpy(ifr.ifr_name, card, sizeof(card));
    int opt = setsockopt(clntSock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    if (opt < 0) {
      DieWithSystemMessage("setsockopt() failed");
    }

    struct sockaddr_in servAddr;
    socklen_t servAddrLen = sizeof(servAddr);
    bzero(&servAddr, servAddrLen);
    servAddr.sin_family = AF_INET;
    // servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, servIP, &servAddr.sin_addr);
    servAddr.sin_port = htons(atoi(servPort));
    printf("start sending data to server...\n");
    ssize_t numBytes = sendto(clntSock, data, dataLen, 0, 
        (struct sockaddr *)&servAddr, servAddrLen);
    if (numBytes < 0) {
      DieWithSystemMessage("sendto() failed");
    } else if (numBytes != dataLen) {
      DieWithUserMessage("sendto() error", "sent unexpected number of bytes");
    }
    struct sockaddr_storage fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
    bzero(buffer, MAX_STRING_LENGTH);
    printf("1\n");
    numBytes = recvfrom(clntSock, buffer, MAX_STRING_LENGTH, 0, 
        (struct sockaddr *)&fromAddr, &fromAddrLen);
    printf("2\n");
    if (numBytes < 0) {
      DieWithUserMessage("recvfrom() error", "received unexpected number of bytes");
    }
    buffer[dataLen] = '\0';
    printf("Received: %s\n", buffer);
    close(clntSock);
  }
}
