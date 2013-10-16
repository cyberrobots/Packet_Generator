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


void *transmitter(void *arg){
	int type,state;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &type);
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&state);
	printf("\n****In Transmitter thread.****\n\n");
	ThreadArg *inTransmit=malloc(sizeof(ThreadArg));
	tra_handler tx_handler;
	mynet	*transmitter_net=(mynet*)malloc(sizeof(mynet));
	void	*buffer;
	uint8_t	*timer;
	uint8_t	*label;
	struct timeval t0,t1;
	struct timeval *timekeep;
	long elapsed;
	unsigned char *etherhead;
	memmove(inTransmit,arg,sizeof(ThreadArg));
	timekeep=malloc(sizeof(struct timeval));
	buffer=(uint8_t*)malloc(sizeof(uint8_t)*BUF_SIZE);
	tx_handler.buffer=buffer;
	tx_handler.inthread=inTransmit;
	tx_handler.transmitter=transmitter_net;
	pthread_cleanup_push(tx_cleanup_handler,(void*)&tx_handler);
	tx_handler.timer=timekeep;
	etherhead=(uint8_t*)buffer;
	transmitter_net->eh=(struct ethhdr*)etherhead;
	/*First init Network Data!*/
	netinit_transmitter(transmitter_net,inTransmit->dst_mac);
	/*Initialize packet before sending.*/
	datainit(buffer,inTransmit->size,transmitter_net->mac,inTransmit->dst_mac);
	timer=buffer+14;
	label=buffer+32;
	sent=0;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&state);
	gettimeofday(&t0, 0);
	while(sent!=NUM_OF_PACKETS){
		gettimeofday(timekeep,NULL);
		time2byte(timekeep,timer);
		label2packet(sent,label);
		sendto(transmitter_net->sock,buffer,inTransmit->size+ETH_HEADER_LEN,0,(struct sockaddr*)&transmitter_net->socket_address,sizeof(transmitter_net->socket_address));
		udelay(inTransmit->delay);
		sent++;
		pthread_testcancel();
	}
	gettimeofday(&t1, 0);
	elapsed = ((t1.tv_sec-t0.tv_sec)*1000000 + (t1.tv_usec-t0.tv_usec));
	printf("\nTransmitter Thread Exiting transmitted: %8i \n",sent);
	printf("***Transmitter Thread Exiting***\n");
	tx_result(inTransmit->path,sent,transmitter_net->sock,inTransmit->loop,inTransmit->delay,inTransmit->size,elapsed);
	/*Delay time in seconds in order to clear network buffers*/
	sleep(inTransmit->thr_interval);
	control=0;
	pthread_cleanup_pop(1);
	//Non zero argument means that the handler will be called even though
	//cancellation did not occur.
	pthread_exit(NULL);
	return NULL;
}

void tx_cleanup_handler(void *arg){
	printf("\n____Inside TX CleaUp Handler____\n");
	tra_handler *in_tx_handler=(tra_handler*)arg;
	close(in_tx_handler->transmitter->sock);
	free(in_tx_handler->buffer);
	free(in_tx_handler->transmitter);
	free(in_tx_handler->inthread);
	free(in_tx_handler->timer);

}

