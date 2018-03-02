/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#define MAXBUFLEN 100
#define SERVERPORT "10020"	// the port users will be connecting to
using namespace std;

struct test
{
  unsigned char tml;
  unsigned char requestId;
  unsigned char opcode;
  char numOperands;
  short op1 __attribute__ ((packed));
  short op2 __attribute__ ((packed));
  //t tml __attribute__ ((packed));
  //t requestId __attribute__ ((packed));
  //t opcode __attribute__ ((packed));
  //t numOperands __attribute__ ((packed));
  //t op1 __attribute__ ((packed));
  //t op2 __attribute__ ((packed));
};
typedef struct test test_t;


int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	char buf[MAXBUFLEN];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	bool operandCheck = true;
	bool secondOpcodeCheck = true;
	bool firstOperandCheck = true;
	int temp;
	test_t var1;
	int numbytes2;
	struct timeval start, end;
	double roundTrip;
	int wow = 1;
	var1.requestId = 1;
	
	while(1) {
	while(operandCheck){
	  printf("Please enter an opcode: ");
	  scanf("%i", &temp);
	  if(temp >= 6 || temp < 0){
	    printf("Incorrect opcode.");
	  }
	  else{
	    var1.opcode = temp;
	    operandCheck = false;
	  }
	}
	while(firstOperandCheck){
	  printf("Please enter the first operand: ");
	  scanf("%d", &temp);
	  if(temp < 0 || temp > 255){
	    printf("Please enter a valid operand(0-255 inclusive)");
	  }
	  else{
	     temp = htons(temp);
	     var1.op1 = temp;
	     var1.numOperands = 1;
	     firstOperandCheck = false;
	   }
	}
	
	while(secondOpcodeCheck){
	  printf("Please enter the second operand: ");
	  scanf("%d", &temp);
	  if(temp < 0 || temp > 255){
	    printf("Please enter a valid operand(0-255 inclusive)");
	  }
	  else{
	    temp = htons(temp);
	    var1.op2 = temp;
	    var1.numOperands = 2;
	    secondOpcodeCheck = false;
	  }
	}
	
	
	
	if (argc != 3) {
	  fprintf(stderr,"usage: talker hostname message\n");
	  exit(1);
	}
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
	  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	  return 1;
	}
	
	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
	  if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
	    perror("talker: socket");
	    continue;
	  }
	  
	  break;
	}
	
	if (p == NULL) {
	  fprintf(stderr, "talker: failed to create socket\n");
	  return 2;
	}
	temp = (int) sizeof(var1);
	var1.tml = temp;
	if ((numbytes = sendto(sockfd, &var1, var1.tml, 0, p->ai_addr, p->ai_addrlen)) == -1) {
	  perror("talker: sendto");
	  exit(1);
	}
	gettimeofday(&start, NULL);
	
	freeaddrinfo(servinfo);
	
	addr_len = sizeof their_addr;
	if ((numbytes2 = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
				  (struct sockaddr *)&their_addr, &addr_len)) == -1) {
	  perror("recvfrom");
	  exit(1);
	}
	gettimeofday(&end, NULL);
	roundTrip = (end.tv_sec - start.tv_sec) * 1000.0;
	roundTrip = (end.tv_usec - start.tv_usec) / 1000.0;
	
	buf[numbytes2] = '\0';
	
	printf("\nThe requestId is %d", buf[1]);
	
	if(buf[2] == 0){
	  int num = (buf[3] << 24) + (buf[4] << 16) + (buf[5] << 8) + buf[6];
	  printf("\nThe result is %04x\n",num);
	}
	else{
	  printf("\nThere was an error with your request");
	}
	
	printf("\nRound trip time: %.3fms", roundTrip);
	char yesOrNo = 'a';
	bool invalid = true;
	while(invalid) {
	printf("\nWould you like to make another request? y/n: ");
	scanf(" %c",&yesOrNo);
	if(yesOrNo == 'y') {
	operandCheck = true;
	firstOperandCheck = true;
	secondOpcodeCheck = true;
	var1.requestId = var1.requestId + 1;
	invalid = false;
	} else if(yesOrNo == 'n') {
	  invalid = false;
	  exit(1);
	}
	}
	close(sockfd);
	}
	return 0;
}
