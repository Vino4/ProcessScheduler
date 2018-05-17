/*
* Matt Almenshad
* Almensha
* Project 2
* CIS 415
*
* This is my own work. Although i did discuss design ideas with a number of students all of whom are mentioned in details in my report.
*/
#include <stdlib.h>
#include <stdio.h>
#include "networkdevice.h"
#include "freepacketdescriptorstore__full.h"
#include "freepacketdescriptorstore.h"
#include "packetdescriptorcreator.h"
#include "packetdescriptor.h"
#include "BoundedBuffer.h"
#include <pthread.h>
#include <unistd.h>
#include "pid.h"
#include "diagnostics.h"

#define UNUSED __attribute__ ((unused))
#define DEFAULT_PACKET_CAPACITY 2

/* any global variables required for use by your threads and your driver routines */

/* network device pointer */
NetworkDevice * nDev;
/* Free Packet Descriptor Store */
FreePacketDescriptorStore * fpdStore;
/* Bounded buffers to store the data */
BoundedBuffer * driverRec[MAX_PID+1];
BoundedBuffer * driverSnd;
/* Thread keys (receiving and sending data) */
pthread_t recThread;
pthread_t sndThread;


/* A thread function which takes in a void pointer to an intially register packet descriptor */
/* Stores received packets into a buffer that is located in the global array 'driverRec'. */
/* uses 'PID' of packet descriptor as index to the global array */
void * receiver(void* blah){
		/* Get the initial packet descriptor ptr, as well as create a temporary variable to get new packet descirptors */
		PacketDescriptor * next = (PacketDescriptor *) blah;
		PacketDescriptor * temp = NULL;
	while(1){
		await_incoming_packet(nDev);

		/* First attempt to get a new packet descritpr */
		if(nonblocking_get_pd(fpdStore, &temp)){
			/* Upon success, write the registered packet descriptor into the appropriate buffer */
			if(nonblockingWriteBB(driverRec[packet_descriptor_get_pid(next)], next)){
				/* Start using the free paket descirptor for the rest of the iteration */
				next = temp;
			} else {
				/* Upon failure to write, put back the extra packet descriptor to the store */
				nonblocking_put_pd(fpdStore, temp);
			}

		}
		/* When fialing to get a new packet descirtpr for the next iteration, re-use the current one */
		/* 	Otherwise, use the newly obtained packet descriptor */

		/* Initialize and register a packet descritpr to receive the next packet from the network */
		init_packet_descriptor(next);
		register_receiving_packetdescriptor(nDev, next);

	}
	return NULL;
}
/* A thread function which handles sending data from the applications to the network */
/* 	Takes data from driverSnd and sends it to the corrosponding  */
void * transmittor(UNUSED void* blah){
	/* track the number of attempts to send a packet*/
	int number_of_tries = 0;
	/* get the next packet from the send buffer */
	PacketDescriptor * next = blockingReadBB(driverSnd);
	DIAGNOSTICS("[Driver]: Sending packet with pid: %d\n", packet_descriptor_get_pid(next));
	while (1){
		number_of_tries++;
		/* if succuessful, put the packet back into the store and get the next one*/
		if(send_packet(nDev, next)){
			DIAGNOSTICS("[Driver]: Sent packet with pid %d after %d attempts.\n", packet_descriptor_get_pid(next), number_of_tries);
			number_of_tries = 0;
			blocking_put_pd(fpdStore, next);
			DIAGNOSTICS("[Driver]: returned packet descriptor descriptor to FPDS.\n");
			next = blockingReadBB(driverSnd);
			DIAGNOSTICS("[Driver]: Sending packet with pid: %d\n", packet_descriptor_get_pid(next));
		} else { /* if failed 3 times, put the packet descriptor back into the store and get the next one */
			if ((number_of_tries+1) >= 3){
				DIAGNOSTICS("[Driver]: repeatidly failed to send a packet with pid %d, giving up.. \n", packet_descriptor_get_pid(next));
				number_of_tries = 0;
				blocking_put_pd(fpdStore, next);
				DIAGNOSTICS("[Driver]: returned packet descriptor to FPDS.\n");	
				next = blockingReadBB(driverSnd);
				DIAGNOSTICS("[Driver]: Sending packet with pid: %d\n", packet_descriptor_get_pid(next));
			} else {
				DIAGNOSTICS("[Driver]: Failed to send packet (%d), reattempting...\n", packet_descriptor_get_pid(next));
			}
		}
	}
	return NULL;
}

/* definition[s] of function[s] required for your thread[s] */
void init_network_driver(NetworkDevice *nd, void *mem_start, unsigned long mem_length, FreePacketDescriptorStore **fpds) {
	/* for tracking the status of creating threads */
	int status;
	int i;
	PacketDescriptor * temp ;
	/* maintain global reference to the network device */
	nDev = nd;
 	/* create Free Packet Descriptor Store */
	fpdStore = create_fpds();
 	/* load FPDS with packet descriptors constructed from mem_start/mem_length */
	create_free_packet_descriptors(fpdStore, mem_start, mem_length);
	/*Register a packet for network device*/
	temp = NULL;
	blocking_get_pd(fpdStore, &temp);
	init_packet_descriptor(temp);
	register_receiving_packetdescriptor(nDev, temp);
 	/* create any buffers required by your thread[s] */
	driverSnd = createBB(10);
	for(i = 0; i < (MAX_PID+1); i++){
		driverRec[i] = createBB(DEFAULT_PACKET_CAPACITY);
	}
 	/* create any threads you require for your implementation */
	status = pthread_create(&sndThread, NULL, &transmittor, NULL);
	if (status){
		DIAGNOSTICS("[Driver]: pThreadCreate_ERROR(%d\n).", status);
		exit(-1);	
	}
	status = pthread_create(&recThread, NULL, &receiver, (void*)temp);
	if (status){
		DIAGNOSTICS("[Driver]: pThreadCreate_ERROR(%d\n).", status);
		exit(-1);	
	}

	/* return the FPDS to the code that called you */
	*fpds = fpdStore;
}

void blocking_send_packet(PacketDescriptor *pd) {
 	/* queue up packet descriptor for sending */
 	/* do not return until it has been successfully queued */
	/*DIAGNOSTICS("[Driver]: received request to send packet with pid %d. (blocking)\n", packet_descriptor_get_pid(pd));*/
	blockingWriteBB(driverSnd, pd);
}

int nonblocking_send_packet(PacketDescriptor *pd) {
	int status;
	/*DIAGNOSTICS("[Driver]: received request to send packet with pid %d. (noblocking)\n", packet_descriptor_get_pid(pd));*/
	status = nonblockingWriteBB(driverSnd, pd);
 	/* if you are able to queue up packet descriptor immediately, do so and return 1 */
	 /* otherwise, return 0 */
	return status;
}

void blocking_get_packet(PacketDescriptor **pd, PID pid) {
	/*DIAGNOSTICS("[Driver]: received request to collect a packet from application %d. (blocking)\n", pid);*/
	*pd = blockingReadBB(driverRec[pid]);
 	/* wait until there is a packet for `pid’ */
 	/* return that packet descriptor to the calling application */
}

int nonblocking_get_packet(PacketDescriptor **pd, PID pid) {
	int status;
	/*DIAGNOSTICS("[Driver]: received request to collect a packet from application %d. (nonblocking)\n", pid);*/
	status = nonblockingReadBB(driverRec[pid], (void*) pd);
 	/* if there is currently a waiting packet for `pid’, return that packet */
 	/* to the calling application and return 1 for the value of the function */
 	/* otherwise, return 0 for the value of the function */
	return status;
}