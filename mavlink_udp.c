/*
 * File name: mavlink_udp.c
 * Purpose: This program sends some data to qgroundcontrol using the mavlink protocol.  The sent packets
 * cause qgroundcontrol to respond with heartbeats.  Any settings or custom commands sent from
 * qgroundcontrol are printed by this program along with the heartbeats.
 * Creating Date: 2019.11.20
 * Author/Charge: Panda Wang (lucky00122@gmail.com)
 * Note: N/A
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#if (defined __QNX__) | (defined __QNXNTO__)
/* QNX specific headers */
#include <unix.h>
#else
/* Linux / MacOS POSIX timer headers */
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h> /* required for the definition of bool in C99 */
#endif

#include <mavlink.h>

#define CLIENT_IP		"127.0.0.1"
#define CLIENT_PORT		14550		// QGroundControl
#define SERVER_PORT		14551		// Vehicle
#define SYSTEM_ID		1
#define COMPONENT_ID	200
#define BUFFER_LENGTH	2041

uint64_t microsSinceEpoch();

int main(int argc, char* argv[])
{
	char help[] = "--help";
	char target_ip[100];
	float position[6] = {10.1, 23.7, 34.5, 3.2, 1.1, 3.3};
	int sock = 0;
	struct sockaddr_in gcAddr; 
	struct sockaddr_in locAddr;
	uint8_t buf[BUFFER_LENGTH];
	ssize_t recsize;
	socklen_t fromlen = sizeof(gcAddr);
	int bytes_sent;
	mavlink_message_t msg;
	uint16_t len;
	int i = 0;

	// Check if --help flag was used
	if((argc == 2) && (strcmp(argv[1], help) == 0))
	{
		printf("\n");
		printf("\tUsage:\n\n");
		printf("\t");
		printf("%s", argv[0]);
		printf(" <ip address of QGroundControl>\n");
		printf("\tDefault for localhost: udp-server 127.0.0.1\n\n");
		exit(EXIT_FAILURE);
	}
	
	// Change the target ip if parameter was given
	if(argc == 2)
	{
		strcpy(target_ip, argv[1]);
	}
	else
	{
		strcpy(target_ip, CLIENT_IP);
	}
	
	// Socket Initial
	if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("[ERR] socket initial failed");
		exit(EXIT_FAILURE);
	}

	/* Attempt to make it non blocking */
#if (defined __QNX__) | (defined __QNXNTO__)
	if(fcntl(sock, F_SETFL, O_NONBLOCK | FASYNC) == -1)
#else
	if(fcntl(sock, F_SETFL, O_NONBLOCK | O_ASYNC) == -1)
#endif
	{
		fprintf(stderr, "[ERR] setting nonblocking: %s\n", strerror(errno));
		close(sock);
		exit(EXIT_FAILURE);
	}
	
	// Server Address config
	memset(&locAddr, 0, sizeof(locAddr));
	locAddr.sin_family = AF_INET;			// AF_UNIX/AF_LOCAL/AF_INET/AF_INET6/PF_INET
	locAddr.sin_addr.s_addr = INADDR_ANY;	// inet_addr("127.0.0.1")
	locAddr.sin_port = htons(SERVER_PORT);
	
	/* Bind the socket to Local Address:SERVER_PORT - necessary to receive packets from qgroundcontrol */ 
	if(bind(sock, (struct sockaddr *)&locAddr, sizeof(struct sockaddr)) == -1)
	{
		perror("[ERR] bind failed");
		close(sock);
		exit(EXIT_FAILURE);
	} 
	
	// Client Address config
	memset(&gcAddr, 0, sizeof(gcAddr));
	gcAddr.sin_family = AF_INET;
	gcAddr.sin_addr.s_addr = inet_addr(target_ip);
	gcAddr.sin_port = htons(CLIENT_PORT);
	
	printf("Start sending/receiving MAVLink message to/from QGroundControl...\n");
	
	while(1) 
	{
		// Send Messages
		
		/* Send Heartbeat HEARTBEAT */
		mavlink_msg_heartbeat_pack(SYSTEM_ID, COMPONENT_ID, &msg, 
									MAV_TYPE_HELICOPTER, MAV_AUTOPILOT_PX4, MAV_MODE_STABILIZE_ARMED, 0, MAV_STATE_ACTIVE);
		len = mavlink_msg_to_send_buffer(buf, &msg);
		bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr));
		
		/* Send Status SYS_STATUS */
		mavlink_msg_sys_status_pack(SYSTEM_ID, COMPONENT_ID, &msg, 
									0, 0, 0, 500, 11000, -1, 100, 0, 0, 0, 0, 0, 0);
		len = mavlink_msg_to_send_buffer(buf, &msg);
		bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof (struct sockaddr));
		
		/* Send Local Position LOCAL_POSITION_NED */
		mavlink_msg_local_position_ned_pack(SYSTEM_ID, COMPONENT_ID, &msg, 
										microsSinceEpoch(), position[0], position[1], position[2], position[3], position[4], position[5]);
		len = mavlink_msg_to_send_buffer(buf, &msg);
		bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr));
		
		/* Send attitude ATTITUDE */
		mavlink_msg_attitude_pack(SYSTEM_ID, COMPONENT_ID, &msg, 
									microsSinceEpoch(), 1.2, 1.7, 3.14, 0.01, 0.02, 0.03);
		len = mavlink_msg_to_send_buffer(buf, &msg);
		bytes_sent = sendto(sock, buf, len, 0, (struct sockaddr*)&gcAddr, sizeof(struct sockaddr));
		
		memset(buf, 0, BUFFER_LENGTH);

		// Receive Messages
		
		recsize = recvfrom(sock, (void *)buf, BUFFER_LENGTH, 0, (struct sockaddr *)&gcAddr, &fromlen);

		if(recsize > 0)
		{
			mavlink_message_t msg;
			mavlink_status_t status;
			
			printf("Bytes Received: %d\nDatagram: ", (int)recsize);

			for(i = 0; i < recsize; i++)
			{
				printf("%02x ", (unsigned char)buf[i]);

				if(mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status) == MAVLINK_FRAMING_OK)
				{
					// Packet received
					printf("\nReceived packet: SYS:%d, COMP:%d, LEN:%d, MSG:%d, SEQ:%d, \n", 
							msg.sysid, msg.compid, msg.len, msg.msgid, msg.seq);
				}
			}
			printf("\n");
		}

		memset(buf, 0, BUFFER_LENGTH);

		sleep(1); // Sleep one second
	}
}

/* QNX timer version */
#if (defined __QNX__) | (defined __QNXNTO__)
uint64_t microsSinceEpoch()
{
	struct timespec time;
	uint64_t micros = 0;
	
	clock_gettime(CLOCK_REALTIME, &time);  
	micros = (uint64_t)time.tv_sec * 1000000 + time.tv_nsec/1000;
	
	return micros;
}
#else
uint64_t microsSinceEpoch()
{
	struct timeval tv;
	uint64_t micros = 0;
	
	gettimeofday(&tv, NULL);  
	micros =  ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;
	
	return micros;
}
#endif
