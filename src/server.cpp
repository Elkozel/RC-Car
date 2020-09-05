#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <RC-Car/Drive.h>
#include "server.h"

// Server definitions
#define PORT 8080
#define BUF_SIZE 256

/**
 * A guard function which looks for errors returned from system calls.
 * 
 * @param check the code returned by the system call
 * @param msg the message to be displayed if the return code indicates an error
 */
int guard(int check, const char *msg)
{
	if (check < 0)
	{
		std::cerr << msg << std::endl;
		exit(errno);
	}
	return check;
}

//! Please change the pins with the ones that are actually in use!
L298N bridge(1, 2, 3, 4, 5, 6, 7, 8); // The H-Bridge controller

int main()
{
	int sockfd;
	char buffer[BUF_SIZE];
	struct sockaddr_in servaddr, cliaddr = {0};

	// Creating socket file descriptor
	sockfd = guard(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), "Failed to create socket");

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	guard(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)), "Binding failed");

	socklen_t addr_len;
	ssize_t msg_len;

	addr_len = sizeof(cliaddr); //len is value/resuslt

	while (true)
	{
		msg_len = guard(recvfrom(sockfd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &addr_len), "Unable to read from socket");
		buffer[msg_len] = '\0';
		printf("Client : %s\n", buffer);
		readCommand(buffer, msg_len);
	}
	return 0;
}

/**
 * Function, which reads the command, parses it and passes it down to the responsible class.
 * 
 * Reading of the command is farely simple, each command consists of 3 bytes:
 * 1 byte acts as a header and two bytes carry the data.
 * 
 * Headers:
 * 1 - X axis
 * 2 - Y axis
 * 5 - Turn on
 * 6 - Turn off
 * 7 - Emergency stop
 * 
 * @param msg the message containing the commands
 * @param size the size of the message
 */
void readCommand(char *msg, size_t size)
{
	for (int s = 0; s < size; s += 3)
	{
		switch (msg[s])
		{
		case 1: // X axis
			bridge.MotorA.setPWM(*(int16_t *)&msg[s + 1]);
			break;
		case 2: // Y axis
			bridge.MotorB.setPWM(*(int16_t *)&msg[s + 1]);
			break;
		case 5: // Turn on
			bridge.enable();
			break;
		case 6: // Turn off
			bridge.disable();
			break;
		case 7: // Emergency stop
			bridge.brake();
			break;
		}
	}
}