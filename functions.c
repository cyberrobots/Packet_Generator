/*
 * functions.c
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#include "libraries.h"
#include "variables.h"
#include "functions.h"


void memory_allocate(){
	/*Path name to create*/
	path						=(char*)malloc(MAX_FILE_NAME*sizeof(char));
	free_path=path;
	/*File names*/
	fname_send_stats			=(char*)malloc(MAX_FILE_NAME*sizeof(char));
	fname_recv_stats			=(char*)malloc(MAX_FILE_NAME*sizeof(char));
	fname_time_analysis			=(char*)malloc(MAX_FILE_NAME*sizeof(char));
	/*Thread structures*/
	Transmitter					=(ThreadArg*)malloc(sizeof(ThreadArg));
	Receiver					=(RcvArg*)malloc(sizeof(RcvArg));
	Transmitter->dst_mac		=(unsigned char*)malloc(sizeof(unsigned char)*ETH_MAC_LEN);
	return;
}

void specification_print(int argc, char *argv[])
{
	printf("\n---System Initialize---\n");
	/*Grab variables from console*/
		if((argc-1)!=10){perror("Variable Error....!");exit(1);}
			else{
				/*Num of streams|Path name|*/
				path=argv[1]; //Copy local the path name;
				NUM_OF_STREAMS				=atoi(argv[2]);
				PACKET_SIZE					=atoi(argv[3]);
				NUM_OF_PACKETS				=atoi(argv[4]);
				STARTING_DELAY				=atoi(argv[5]);
				STREAM_INTERVAL				=atoi(argv[6]);
				Transmitter->thr_interval	=atoi(argv[7]);
				DEVICE						=(unsigned char*)argv[8];
				DEVICE2						=(unsigned char*)argv[9];
				/*Grab the MAC Address for Console.*/
				if(mac_import(argv[10],Transmitter->dst_mac)!=0)
				{
					perror("Wrong MAC");
					exit(1);
				}
				printf("MAC OK....!\n");

			}
		printf("\nCreating path ./%s\n",path);
		if (!mkdir(path,0777)) {
			printf("PATH CREATION SUCCESS\n");
		} else {
			printf("PATH CREATION FAILURE OR PRE_EXISTED \n");
		}
		/*Specifications Print*/
		printf("\nThe Numbers of Loops is:%8i.\n",NUM_OF_STREAMS);
		printf("The Packet size is:\t%8i.\n",PACKET_SIZE);
		printf("Number of packet is:\t%8i.\n",NUM_OF_PACKETS);
		printf("Starting Delay is:\t%8i.\n",STARTING_DELAY);
		printf("Stream interval is:\t%8i.\n",STREAM_INTERVAL);
		printf("Time for next Stream is:%8i.\n",Transmitter->thr_interval);
		/*Pass values and pointers to threads*/
		Transmitter->delay	=STARTING_DELAY;
		Transmitter->size	=PACKET_SIZE;
		Transmitter->path	=&path;
		Receiver->size		=PACKET_SIZE;
		Receiver->path		=&path;
		/*Allocate space for the time statics file printing.*/
		Array=(time_char*)calloc(NUM_OF_PACKETS,sizeof(time_char));
		return;
}

int mac_import(char *input,unsigned char *OUT)
{
	unsigned char MAC_in[6];
	int i;

	if(sscanf(input,"%2X:%2X:%2X:%2X:%2X:%2X",&MAC_in[0],&MAC_in[1],&MAC_in[2],&MAC_in[3],&MAC_in[4],&MAC_in[5])!=6){
		printf("Put the MAC in proper format XX:XX:XX:XX:XX:XX.\n");
		return 1;/*Wrong input code.*/
	}
	printf("Target MAC");
	for(i=0;i<6;i++){
		printf(":%2X",MAC_in[i]);
	}
	printf("\n");
	memcpy(OUT,&MAC_in,6);
	return 0; /*Success Code*/
}

void sigint(int signum){
	/*Application Kill CleanUp Function.*/
	printf("Caught signal %d\n",signum);
	/*Cancel Threads*/
	pthread_cancel(Transmitter_thr);
	pthread_join(Transmitter_thr,statusTR);
	pthread_cancel(Receiver_thr);
	pthread_join(Receiver_thr,statusRC);
	printf("Packets transmitted	: %8i.\n",sent);
	printf("Packets received	: %8i.\n",rec);
	printf("\nPacket Generator terminated By User....\n");
	exit(1);
}


void finish(){
	/*Normally Finish CleanUp Function.*/
	printf("\nPacket Generator Finished Normally...\n");
	free(fname_send_stats);
	free(fname_recv_stats);
	free(fname_time_analysis);
	free(Transmitter->dst_mac);
	free(Transmitter);
	free(Receiver);
	free(Array);
	free(free_path);
	pthread_attr_destroy(&attr);
	return;
}



void netinit_transmitter(mynet *transmitter,unsigned char* dst_mac){
	/*Network Initialize Transmitter Code*/
	unsigned char src_mac[6];
	int j;
	int ifindex=0;
	/*Open RAW socket*/
	transmitter->sock=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	/*Grab interface name.*/
	strncpy(transmitter->ifr.ifr_name,(const char*)DEVICE,IFNAMSIZ);
	/*Grab interface index from kernel.*/
	if (ioctl(transmitter->sock, SIOCGIFINDEX,&transmitter->ifr)==-1) {perror("SIOCGIFINDEX");exit(1);}
	ifindex=transmitter->ifr.ifr_ifindex;
	/*Grab interface's with above index MAC address.*/
	if(ioctl(transmitter->sock,SIOCGIFHWADDR,&transmitter->ifr)==-1){perror("SIOCGIFHWADDR");exit(1);}
	transmitter->socket_address.sll_family   = PF_PACKET;
	transmitter->socket_address.sll_protocol = htons(ETH_P_IP);
	transmitter->socket_address.sll_ifindex  = ifindex;
	transmitter->socket_address.sll_hatype   = ARPHRD_ETHER;
	transmitter->socket_address.sll_pkttype  = PACKET_OTHERHOST;
	transmitter->socket_address.sll_halen    = ETH_ALEN;
	transmitter->socket_address.sll_addr[0]  = dst_mac[0];
	transmitter->socket_address.sll_addr[1]  = dst_mac[1];
	transmitter->socket_address.sll_addr[2]  = dst_mac[2];
	transmitter->socket_address.sll_addr[3]  = dst_mac[3];
	transmitter->socket_address.sll_addr[4]  = dst_mac[4];
	transmitter->socket_address.sll_addr[5]  = dst_mac[5];
	transmitter->socket_address.sll_addr[6]  = 0x00;
	transmitter->socket_address.sll_addr[7]  = 0x00;
	for(j=0;j<6;j++){src_mac[j]=transmitter->ifr.ifr_hwaddr.sa_data[j];}
#ifdef Debug
	printf("Successfully got our MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);
#endif
	memcpy(&transmitter->mac,&src_mac,ETH_MAC_LEN);
	transmitter->eh->h_proto=ETH_P_NULL;
	return;
}

void datainit(void *buffer,int size,unsigned char *src_mac,unsigned char *dst_mac)
{
	/*Initialize Data*/
	int i;
	unsigned char *data;
	data=(uint8_t*)buffer+14;
	static_memcpy(buffer,src_mac,dst_mac);
	for(i=0;i<size;i++){data[i]=(unsigned char)170;}
	return;
}

void static_memcpy(uint8_t *buffer, unsigned char *src_mac,unsigned char *dst_mac)
{
	/*||Destination_mac||Source_mac||Protocol||Data||CRC||*/
	memmove(buffer,dst_mac,ETH_MAC_LEN);
	memmove(buffer+ETH_MAC_LEN,(void*)src_mac,ETH_MAC_LEN);
	return;
}

void label2packet(int label,uint8_t* buffer){
	//label=htonl(label); /*Take care for little endian and big endian systems*/
	memmove(buffer,&label,sizeof(int));
	return;
}

void time2byte(struct timeval *input, uint8_t* output){
	/*Convert time stamp to byte ordering for placing on the packet.*/
	//printf("\nIN time2byte FUNCTION\n");
	//printf("%8.X__%8.X\n",input->tv_sec,input->tv_usec);
	*(output+15)	=(input->tv_usec&0x0000F);
	*(output+14)	=(input->tv_usec&0x000F0)>>4;
	*(output+13)	=(input->tv_usec&0x00F00)>>8;
	*(output+12)	=(input->tv_usec&0x0F000)>>12;
	*(output+11)	=(input->tv_usec&0xF0000)>>16;
	*(output+10)	=(input->tv_usec&0xF0000)>>20;
	*(output+9)		=(input->tv_usec&0xF0000)>>24;
	*(output+8)		=(input->tv_usec&0xF0000)>>28;
	*(output+7)		=(input->tv_sec&0x0000000F);
	*(output+6)		=(input->tv_sec&0x000000F0)>>4;
	*(output+5)		=(input->tv_sec&0x00000F00)>>8;
	*(output+4)		=(input->tv_sec&0x0000F000)>>12;
	*(output+3)		=(input->tv_sec&0x000F0000)>>16;
	*(output+2)		=(input->tv_sec&0x00F00000)>>20;
	*(output+1)		=(input->tv_sec&0x0F000000)>>24;
	*(output+0)		=(input->tv_sec&0xF0000000)>>28;
	//printf("%X %X %X %X %X %X %X %X %X %X %X %X %X\n",*(output+0),*(output+1),*(output+2),*(output+3),*(output+4),*(output+5),*(output+6),*(output+7),*(output+8),*(output+9),*(output+10),*(output+11),*(output+12));
	return;
}

void byte2time(uint8_t* input, struct timeval *output){
	/*Reversing the time2byte in order to calculate travel time.*/
	//printf("\nIN byte2time FUNCTION\n");
	//printf("%X %X %X %X %X %X %X %X %X %X %X %X %X\n",*(input+0),*(input+1),*(input+2),*(input+3),*(input+4),*(input+5),*(input+6),*(input+7),*(input+8),*(input+9),*(input+10),*(input+11),*(input+12));
	output->tv_sec=*(input);
	output->tv_sec=(output->tv_sec<<4)|*(input+1);
	output->tv_sec=(output->tv_sec<<4)|*(input+2);
	output->tv_sec=(output->tv_sec<<4)|*(input+3);
	output->tv_sec=(output->tv_sec<<4)|*(input+4);
	output->tv_sec=(output->tv_sec<<4)|*(input+5);
	output->tv_sec=(output->tv_sec<<4)|*(input+6);
	output->tv_sec=(output->tv_sec<<4)|*(input+7);
	output->tv_usec=(output->tv_usec<<4)|*(input+8);
	output->tv_usec=(output->tv_usec<<4)|*(input+9);
	output->tv_usec=(output->tv_usec<<4)|*(input+10);
	output->tv_usec=(output->tv_usec<<4)|*(input+11);
	output->tv_usec=(output->tv_usec<<4)|*(input+12);
	output->tv_usec=(output->tv_usec<<4)|*(input+13);
	output->tv_usec=(output->tv_usec<<4)|*(input+14);
	output->tv_usec=(output->tv_usec<<4)|*(input+15);
	//printf("%8.X__%8.X\n",output->tv_sec,output->tv_usec);
	return;
}

void udelay(long microns){
	/*Delay function with microseconds accuracy*/
	struct timeval delay;
	struct timeval target;
	struct timeval now;
	delay.tv_sec = 0;
	delay.tv_usec = microns;
	while (delay.tv_usec >= 1000000)
	{
		delay.tv_sec++;
		delay.tv_usec -= 1000000;
	}

	gettimeofday(&now, NULL);
	timeradd(&now, &delay, &target);

	do {
		gettimeofday(&now, NULL);
	} while (timercmp(&now, &target,<));

}

void rx_result(char **rx_path,int rec,int rx_sock,int loop, int size)
{
		/*Receive statistics file open*/
		int i;
		char loop_num[33];
		char packet_size[33];
		long temp;
		snprintf(loop_num, sizeof(loop_num), "%d", loop);
		snprintf(packet_size, sizeof(packet_size), "%d", size);
		strcpy(fname_recv_stats,"./");
		strcat(fname_recv_stats,*rx_path);
		strcat(fname_recv_stats,"/stream_");
		strcat(fname_recv_stats,loop_num);
		strcat(fname_recv_stats,"_size_");
		strcat(fname_recv_stats,packet_size);
		strcpy(fname_time_analysis,fname_recv_stats);
		strcat(fname_recv_stats,"_recv_stats.txt");
		strcat(fname_time_analysis,"_time_analysis.m");
		/*File open & descriptor*/
		recv_stats		=fopen(fname_recv_stats,"w");
		if(recv_stats==NULL){perror("No File or Bad Directory");exit(1);}
		time_analysis	=fopen(fname_time_analysis,"w");
		if(time_analysis==NULL){perror("No File or Bad Directory");exit(1);}
		/*File printing*/
		fprintf(recv_stats,"\nResults From Channel Tester.(Receive)\n");
		fprintf(recv_stats,"Iteration              :\t%8i.\n",loop);
		fprintf(recv_stats,"For Socket             :\t%8i.\n",rx_sock);
		fprintf(recv_stats,"Data Size              :\t%8i.\n",size);
		fprintf(recv_stats,"Packets Recv           :\t%8i.\n",rec);
		fprintf(time_analysis,"%%Timining Analysis Report For MATLAB processing!\n\n");
		fprintf(time_analysis,"vector%i=[\n",loop);
		temp=Array[0].INTERARRIVAL_TIME;
		fprintf(time_analysis,"%7i,%8li,%12li;\n",Array[0].ID,Array[0].LATENCY,(Array[0].INTERARRIVAL_TIME-temp));
		for(i=1;i<NUM_OF_PACKETS;i++){
			if(Array[i].ID!=0x00 && Array[i].INTERARRIVAL_TIME!=0 && Array[i].LATENCY!=0){
				temp=Array[i-1].INTERARRIVAL_TIME;
				fprintf(time_analysis,"%7i,%8li,%12li;\n",Array[i].ID,Array[i].LATENCY,(Array[i].INTERARRIVAL_TIME-temp));
			}
		}

		fprintf(time_analysis,"];\n");
		/*Close files*/
		fclose(recv_stats);
		fclose(time_analysis);
		/*Console Printing*/
		printf("\nResults From Channel Tester.(Receive)\n");
		printf("Iteration              :\t%8i.\n",loop);
		printf("For Socket             :\t%8i.\n",rx_sock);
		printf("Data Size              :\t%8i.\n",size);
		printf("Packets Received       :\t%8i.\n",rec);
}


void tx_result(char **tx_path,int sent,int tx_sock,int loop, int delay, int size,long elapsed)
{
	/*Filename and path fix*/
	char loop_num[33];
	char packet_size[33];
	snprintf(loop_num, sizeof(loop_num), "%d", loop);
	snprintf(packet_size, sizeof(packet_size), "%d", size);
	strcpy(fname_send_stats,"./");
	strcat(fname_send_stats,*tx_path);
	strcat(fname_send_stats,"/stream_");
	strcat(fname_send_stats,loop_num);
	strcat(fname_send_stats,"_size_");
	strcat(fname_send_stats,packet_size);
	strcat(fname_send_stats,"_send_stats.txt");
	/*File open&descriptor*/
	send_stats		=fopen(fname_send_stats,"w");
	if(send_stats==NULL){perror("No File or Bad Directory");exit(1);}
	/*File Print*/
	fprintf(send_stats,"\nResults From Channel Tester.\n");
	fprintf(send_stats,"Iteration              :\t%8i.\n",loop);
	fprintf(send_stats,"For Socket             :\t%8i.\n",tx_sock);
	fprintf(send_stats,"Delay                  :\t%8i.\n",delay);
	fprintf(send_stats,"Data Size              :\t%8i.\n",size);
	fprintf(send_stats,"Time of Receiving (us) :\t%8li.\n",elapsed);
	fprintf(send_stats,"Packet Rate (PPS)      :\t%8.0f.\n",(float)(NUM_OF_PACKETS/(elapsed/1000000)));
	fprintf(send_stats,"Packets Sent           :\t%8i.\n",sent);
	/*Close file*/
	fclose(send_stats);
	/*Console print*/
	printf("\nResults From Channel Tester.(Transmit)\n");
	printf("Iteration              :\t%8i.\n",loop);
	printf("For Socket             :\t%8i.\n",tx_sock);
	printf("Delay                  :\t%8i.\n",delay);
	printf("Data Size              :\t%8i.\n",size);
	printf("Time of Receiving (us) :\t%8li.\n",elapsed);
	printf("Packet Rate (PPS)      :\t%8.0f.\n",(float)(NUM_OF_PACKETS/(elapsed/1000000)));
	printf("Packets Sent           :\t%8i.\n",sent);
	printf("\n");
}

void netinit_receiver(mynet *receiver){
	int ifindex_rec,j;
	//unsigned char	src_mac_rec[6];
	receiver->sock=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	strncpy(receiver->ifr.ifr_name,(const char*)DEVICE2,IFNAMSIZ);
	if (ioctl(receiver->sock, SIOCGIFINDEX,&receiver->ifr)==-1) {perror("SIOCGIFINDEX");exit(1);}
	ifindex_rec=receiver->ifr.ifr_ifindex;
	if(ioctl(receiver->sock,SIOCGIFHWADDR,&receiver->ifr)==-1){perror("SIOCGIFHWADDR");exit(1);}
	if(ioctl(receiver->sock, SIOCGIFFLAGS, &receiver->ifr)==-1){perror("SIOCGIFFLAGS");exit(1);}
	receiver->ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(receiver->sock, SIOCGIFFLAGS, &receiver->ifr)==-1){perror("SIOCGIFFLAGS");exit(1);}
	receiver->socket_address.sll_family   = PF_PACKET;
	receiver->socket_address.sll_protocol = htons(ETH_P_IP);
	receiver->socket_address.sll_ifindex  = ifindex_rec;
	receiver->socket_address.sll_hatype   = ARPHRD_ETHER;
	receiver->socket_address.sll_pkttype  = PACKET_OTHERHOST;
	receiver->socket_address.sll_halen    = ETH_ALEN;
	receiver->socket_address.sll_addr[6]  = 0x00;
	receiver->socket_address.sll_addr[7]  = 0x00;
	for(j=0;j<6;j++){
		src_mac_rec[j]=receiver->ifr.ifr_hwaddr.sa_data[j];
		//src_mac_rec[j]=receiver->ifr.ifr_hwaddr.sa_data[j];
	}
	//memcpy(mynet.src_mac_rec,ETH_MAC_LEN);
#ifdef DEBUG
	printf("Socket for Receiver with ID: %i created.\n",receiver->sock);
	printf("Successfully got our MAC address (receive interface): %02X:%02X:%02X:%02X:%02X:%02X\n",
					src_mac_rec[0],src_mac_rec[1],src_mac_rec[2],
					src_mac_rec[3],src_mac_rec[4],src_mac_rec[5]);
#endif

}
