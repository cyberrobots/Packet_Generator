/*
 * functions.h
 *
 *  Created on	: Jul 4, 2013
 *  Finished on	: Jul 10 2013
 *  Author		: Sotiris Lyras
 *  Version		: v04
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void memory_allocate();
void specification_print(int argc, char *argv[]);

/*Global*/
void udelay(long microns);
void sigint(int signum);
void finish();
int mac_import(char *input, unsigned char *out);

/*Threads Functions*/
void *transmitter(void *arg);
void tx_cleanup_handler(void *arg);

void *receiver(void *arg_rec);
void rx_cleanup_handler(void *arg);

/*In Transmitter Thread Functions*/
void netinit_transmitter(mynet *transmitter,unsigned char* dst_mac);
void netinit_receiver(mynet *receiver);
void time2byte(struct timeval *input, uint8_t* output);
void byte2time(uint8_t* input, struct timeval *output);
void label2packet(int label,uint8_t* buffer);
void datainit(void *buffer,int size,unsigned char *src_mac,unsigned char *dst_mac);
void time2byte(struct timeval *input, uint8_t* output);
void byte2time(uint8_t* input, struct timeval *output);
void static_memcpy(uint8_t *buffer, unsigned char *src_mac,unsigned char *dst_mac);
void label2packet(int label,uint8_t* buffer);

/*Statistics Print*/
void tx_result(char **tx_path,int sent,int tx_sock,int loop, int delay, int size,long elapsed);
void rx_result(char **rx_path,int rec,int rx_sock,int loop, int size);
#endif /* FUNCTIONS_H_ */
