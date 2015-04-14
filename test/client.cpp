//Socket client
#include <sys/types.h>
#include<sys/socket.h>
#include<unistd.h> //for read/write
#include<netdb.h>
#include<string.h> //for memset
#include<stdio.h> //for fprintf, stderr, etc.
#include<stdlib.h> //for exit

int main()
{
  int sockfd; //file descriptor for the socket
  int status; //for error checking

  //change this # between 2000-65k before using
  const char* portno="42424"; 

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints)); //zero out everything in structure
  hints.ai_family = AF_UNSPEC; //don't care. Either IPv4 or IPv6
  hints.ai_socktype=SOCK_STREAM; // TCP stream sockets

  struct addrinfo *servinfo;
  //Can use a host name or an ip address
  if((status=getaddrinfo("127.0.0.1", portno, &hints, &servinfo))==-1)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  if((sockfd=socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol))==-1)
  {
    perror("socket");
    exit(1);
  }
  if((status=connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen))==-1)
  {
    perror("connect");
    exit(1);
  }
  //release the information allocated by getaddrinfo()
  freeaddrinfo(servinfo);

  for(int i=0; i<5; ++i)
  {
    const char* message;
    switch(i)
    {
      case 0:
        message="zero";
        break;
      case 1:
        message="one";
        break;
      case 2:
        message="two";
        break;
      case 3:
        message="three";
        break;
      case 4:
        message="four";
        break;
    }
    int n;
    if((n=write(sockfd, message, strlen(message)))==-1)
    {
      perror("write");
      exit(1);
    }
    printf("client wrote %d characters\n", n);
    char dummybuffer[10];
    printf("\nPress <Enter> to continue.\n");
    fgets(dummybuffer,9,stdin);
  }
  close(sockfd);
  return 0;
}
