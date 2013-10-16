/*
 * thread_receive.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#include "libraries.h"
#include "variables.h"
#include "functions.h"


void *receiver(void *arg_rec){
	printf("****In Receiver thread.****\n");
	int type,state;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &type);
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
	RcvArg *inrcv	=(RcvArg*)malloc(sizeof(RcvArg));
	mynet *receiver	=(mynet*)malloc(sizeof(mynet));
	void *inbuffer	=(uint8_t*)malloc(BUF_SIZE*sizeof(uint8_t));
	struct timeval	*timediff0;
	struct timeval	*timediff1;
	uint8_t			*timer; //Points the packets time stamp.
	uint8_t 		*label; //Points the packets label id stamp.
	rec_handler rx_handler;
	/*Time structures*/
	timediff0=malloc(sizeof(struct timeval)); // The present time.
	timediff0->tv_sec=0;
	timediff0->tv_usec=0;
	timediff1=malloc(sizeof(struct timeval)); // Packets departure time.
	timediff1->tv_sec=0;
	timediff1->tv_usec=0;
	unsigned char *recetherhead;
	recetherhead=(uint8_t*)inbuffer;
	/*Pointers for time and label.*/
	timer=recetherhead+14;
	label=recetherhead+32;
	receiver->eh=(struct ethhdr*)recetherhead;
	/*Copy locally thread variables.*/
	memmove(inrcv,arg_rec,sizeof(RcvArg));
	rx_handler.inbuffer=inbuffer;
	rx_handler.inrcv=inrcv;
	rx_handler.receiver=receiver;
	rx_handler.timer0=timediff0;
	rx_handler.timer1=timediff1;
	pthread_cleanup_push(rx_cleanup_handler,(void*)&rx_handler);
	/*Initialize Receiver Function.*/
	netinit_receiver(receiver);
	rec=0;
	while(control){
		recvfrom(receiver->sock,inbuffer,BUF_SIZE,0,NULL,NULL);
		gettimeofday(timediff0,NULL);
		if (receiver->eh->h_proto == ETH_P_NULL && memcmp( (const void*)receiver->eh->h_dest,(const void*)src_mac_rec,ETH_MAC_LEN) == 0)
		{
			/*Convert bytes to time.*/
			byte2time(timer,timediff1);
			memmove(&Array[rec].ID,label,sizeof(unsigned int));
			Array[rec].LATENCY=(((timediff0->tv_sec-timediff1->tv_sec)*1000000)+(timediff0->tv_usec-timediff1->tv_usec));
			Array[rec].INTERARRIVAL_TIME=((timediff0->tv_sec*1000000)+timediff0->tv_usec);
			//Received Counter.
			rec++;
			timediff1->tv_sec=0;
			timediff1->tv_usec=0;
		}
		else{
			;
		}
	}
	printf("\nReceiver Thread Exiting Received:       %8i \n",rec);
	printf("***Receiver Thread Exiting***\n");
	/*Write output file function.*/
	rx_result(inrcv->path,rec,receiver->sock,inrcv->loop,inrcv->size);
	pthread_cleanup_pop(1);
	pthread_exit(NULL);
	return NULL;
}

void rx_cleanup_handler(void *arg){
	rec_handler *in_rx_handler=(rec_handler*)arg;
	close(in_rx_handler->receiver->sock);
	free(in_rx_handler->inrcv);
	free(in_rx_handler->inbuffer);
	free(in_rx_handler->receiver);
	free(in_rx_handler->timer0);
	free(in_rx_handler->timer1);
	printf("\n____Inside RX CleaUp Handler____\n\n");
}
