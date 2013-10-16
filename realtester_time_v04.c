/*
 * realtester_time_v02.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

/* --------------------------------------------------
 * Files.
 * --------------------------------------------------
 * Source Files: threads.c, realtester_time_v02.c functions.c
 * Header Files: libraries.h, variables.h, functions.h
 * --------------------------------------------------
 * Code Description.
 * --------------------------------------------------
 * This Packet generator produces RAW packets with destination defined in
 * dst_mac[] char vector. It keeps statistics for packets latency and
 * inter arrival time between two packets in order to collect data for
 * Jitter and Packet Delay Variation (PDV).
 * --------------------------------------------------
 * --------------------------------------------------
 * Imports (Mandatory).
 * --------------------------------------------------
 * In order to execute you must provide the following:
 * --------------------------------------------------
 * 1)	Path name to store files in.
 * 2)	Number of streams user would like to produce.
 * 3)	The size of packets user would like to send.
 * 4)	The delay (in uSec) packets should have between them.(fix the rate (PPS)).
 * 5)	The delay (in uSec) interval which is going to be added in each stream,
 * 		if the user wants different delay for each stream.
 * 6)	The time interval (in Seconds) between two streams.
 * 7)	Interface for receiving traffic.
 * 8)	Interface for sending traffic.
 * 9)	MAC address for the target machine. Format: "xx:xx:xx:xx:xx:xx".
 * --------------------------------------------------
 * Exports.
 * 1)	stream characteristics *_send_stats.txt
 * 2)	stream characteristic  *_recv_stats.txt
 * 3)	stream characteristic  *_time_analysis.txt
 * --------------------------------------------------
 */
/*Libraries*/
#include "libraries.h"
#include "variables.h"
#include "functions.h"
/*Main functions*/
int main(int argc, char *argv[]){
	printf("\n***Starting The Generator***\n");
	/*Pthread init*/
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	memory_allocate(); 				//Allocate memory for the system.
	specification_print(argc,argv);	//Print systems specification.
	signal(SIGINT,sigint); 			//Set the signal.
	int i;
	for(i=0;i<NUM_OF_STREAMS;i++){
		Transmitter->loop=i;
		Receiver->loop=i;
		control=1;
		/*Fire up threads*/
		pthread_create(&Receiver_thr,&attr,receiver,Receiver);
		pthread_create(&Transmitter_thr,&attr,transmitter,Transmitter);
		/*Wait to finish the send.*/
		pthread_join(Transmitter_thr,statusTR);
		pthread_join(Receiver_thr,statusRC);
		Transmitter->delay=Transmitter->delay+STREAM_INTERVAL;
	}
	/*Memory cleanup function*/
	finish();
	return 0;
}
