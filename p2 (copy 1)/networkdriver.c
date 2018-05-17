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

/*global variables*/
FreePacketDescriptorStore *fpds;		/*free packet descriptor store*/
BoundedBuffer *receivingBB[MAX_PID+1];  /*receiving buffer array*/
BoundedBuffer *sendingBB;				/*sending buffer */
NetworkDevice *netDev;

pthread_t s_threadID;					/*pthread ids*/
pthread_t r_threadID;					
				

/*function definitions*/
static void* receiveyfunction(UNUSED void *unused);
static void* sendyfunction(void *unused);
/* any global variables required for use by your threads and your driver routines */
/* definition[s] of function[s] required for your thread[s] */

void init_network_driver(NetworkDevice *nd, void *mem_start,
 unsigned long mem_length, FreePacketDescriptorStore **_fpds) {
		/* create Free Packet Descriptor Store */
	int i;
	netDev = nd;
	fpds = create_fpds();

 		/* load FPDS with packet descriptors */
	create_free_packet_descriptors(fpds, mem_start, mem_length);
 
		/* create sending and receiving buffers */
	sendingBB = createBB(12);
		/* MAX_PID+1 receiving buffers */

	for(i=0; i<MAX_PID+1; i++){
		receivingBB[i] = createBB(2);
	}

		/* create any threads you require for your implementation */
		/*receive thread*/
	pthread_create(&s_threadID, NULL, &sendyfunction, NULL);
	pthread_create(&r_threadID, NULL, &receiveyfunction, NULL);
		/*sending threads...?*/
/*=================== create sending threads*/
	 /* return the FPDS to the code that called you */
	*_fpds = fpds;
/*=================== what does this mean*/
}


static void* sendyfunction(UNUSED void *unused){	
	int forever = 1;
	PacketDescriptor *pd;
	int result;
	while(forever){
		pd = blockingReadBB(sendingBB);
		result = send_packet(netDev, pd);
		/* if send_packet fails */
		if (!result){				
			if(!send_packet(netDev, pd)){ /* if send_packet fails */
				send_packet(netDev, pd);	/* try one last time */		
			}
		}
		/*send packet descriptor back to FPDS */
		nonblocking_put_pd(fpds, pd);	
	}
	return NULL;
}

static void* receiveyfunction(UNUSED void *unused){
		/* init empty packet descriptor pointer */
	PacketDescriptor *pd = NULL;
	PacketDescriptor *pd2 = NULL;

/*get pd to get 1st packet*/
		//blocking_get_packet(PacketDescriptor **pd, PID pid);	
	blocking_get_pd(fpds, &pd);
/*should the rest of these be in some kind of loop or something...? or should the */
	//initial pd get be not in this function?
	while(1){
			init_packet_descriptor(pd);
			register_receiving_packetdescriptor(netDev, pd);
			await_incoming_packet(netDev);
		/*non-blocking get pd for anther pd */
		if (nonblocking_get_pd(fpds, &pd2)){
		/* successfully got another pd */
			if(!nonblockingWriteBB(receivingBB[packet_descriptor_get_pid(pd)], pd))
				/* failed to write pd to receiving buffer, so put it back in fpds */
				nonblocking_put_pd(fpds, pd);

			pd = pd2;
			/*do non-blocking put
			if that^^ fails:
				return one packet to the FreeStore
				initialize the one you have left
				register
				await  */				
		}
	}
	return NULL;
}

void blocking_send_packet(PacketDescriptor *pd) {
 /* queue up packet descriptor for sending */
 /* do not return until it has been successfully queued */
	blockingWriteBB(sendingBB, pd);
}
int nonblocking_send_packet(PacketDescriptor *pd) {
 /* if you are able to queue up packet descriptor immediately, do so and return 1 */
 /* otherwise, return 0 */
	return nonblockingWriteBB(sendingBB, pd);
}
void blocking_get_packet(PacketDescriptor **pd, PID pid) {
 /* wait until there is a packet for `pid’ */
 /* return that packet descriptor to the calling application */
	*pd = (PacketDescriptor *)blockingReadBB(receivingBB[pid]);
}
int nonblocking_get_packet(PacketDescriptor **pd, PID pid) {
 /* if there is currently a waiting packet for `pid’, return that packet */
 /* to the calling application and return 1 for the value of the function */
 /* otherwise, return 0 for the value of the function */
	return nonblockingReadBB(receivingBB[pid], (void *)pd);
}