/*
 * variables.h
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

/*Basic Defines for Network Interface*/
#define ETH_HEADER_LEN	ETH_HLEN	/*Total octets in header.*/
#define BUF_SIZE		1518
#define ETH_FRAME_		TOTALLEN	1518
#define ETH_MAC_LEN		ETH_ALEN
#define ETH_P_NULL		0x0808 /*Packet Protocol, self defined/don't care.*/

/*Depending you distro you must put here the proper interface name.*/
//#define DEVICE			"p5p1" /*Interface_1*/
//#define DEVICE2			"p5p1" /*Interface_2*/


#define MAX_FILE_NAME 			50		/*The maximum size for filename*/
#define MAX_PACKET_INT_SEC 		1		/*maximum time packet interval*/
#define MAX_PACKET_INT_USEC 	0


/*Type Definitions*/
typedef struct {
	long int		delay;
	int				size;
	int				loop;
	int				thr_interval;
	char			**path;
	unsigned char	*dst_mac;
}ThreadArg;

typedef struct {
	int		loop;
	int		size;
	char	**path;
}RcvArg;

typedef struct {
	int 				sock;
	struct	sockaddr_ll	socket_address;
	struct	ifreq		ifr;
	struct	ethhdr		*eh;
	unsigned char		mac[6];
	unsigned char		my_mac[6];
}mynet;

typedef struct{
	ThreadArg		*inthread;
	void			*buffer;
	mynet			*transmitter;
	struct timeval	*timer;
	FILE 			*sent_stats;
}tra_handler;

typedef struct{
	RcvArg			*inrcv;
	void			*inbuffer;
	mynet			*receiver;
	struct timeval	*timer0;
	struct timeval	*timer1;
}rec_handler;

typedef struct{
	unsigned int	ID;
	long			LATENCY;
	long			INTERARRIVAL_TIME;
}time_char;


/*Global Variables*/
unsigned char	*DEVICE;
unsigned char	*DEVICE2;
unsigned char	src_mac_rec[6];
ThreadArg		*Transmitter;
RcvArg			*Receiver;
unsigned int	NUM_OF_PACKETS;
unsigned int	STARTING_DELAY;
unsigned int	STREAM_INTERVAL;
uint8_t			control;

/*Packet Counters*/
unsigned int	sent;
unsigned int	rec;

/*Input Data*/
int	NUM_OF_STREAMS;
int	PACKET_SIZE;

/*Threads*/
pthread_t		Transmitter_thr;
pthread_t		Receiver_thr;
pthread_attr_t	attr;

void *statusTR;
void *statusRC;

/*Statistics*/
time_char *Array;

/*Files*/
FILE	*send_stats;
FILE	*recv_stats;
FILE	*time_analysis;
/*Strings for files labeling*/
char	*path;
char	*free_path;
char	*fname_send_stats;
char	*fname_recv_stats;
char	*fname_time_analysis;

#endif /* VARIABLES_H_ */
