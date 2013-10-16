Packet_Generator
================

/* 
 *  Finished on	: Jul 10 2013
 *  Author		  : Sotiris Lyras
 *  Version		  : v04
 *  Email       : cyberrobots@gmail.com
 *  Feel free contanting me.
 */


/*
 * This is a MAC layer "train/burst" Packet generator which produces RAW packets with destination defined in dst_mac char vector. 
 * It keeps statistics for packets latency and inter arrival time between two packets,
 * in order to collect data for Packet Delay Variation (PDV).
 */

/* --------------------------------------------------
 * Files.
 * --------------------------------------------------
 * Source Files: thread_receive.c, thread_transmit.c, realtester_time_v04.c functions.c
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
 * 5)	The delay (in uSec) interval which is going to be added in each stream, if the user wants different delay for each stream.
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
 * Compilation.
 * gcc thread_receive.c thread_transmit.c realtester_time_v04.c functions.c -lpthread -lrt -D_REENTRANT -o NAME
 * --------------------------------------------------
 * Running.
 * WARNING: sudo because this is a mac socket app, requires su in order to take info from kernel.
 * sudo ./NAME PATH_TO_SAVE NUM_OF_STREAMS PACKET_LEN NUM_OF_PACKETS DELAY_BET_PACKETS  TIME_INT_FOR_NEXT WAIT_TIME_FOR_NEXT_STREAM STREAM  IN_INTERFACE OUT_INTERFACE xx:xx:xx:xx:xx:xx
 * --------------------------------------------------
 * Example.
 * eg: gcc thread_receive.c thread_transmit.c realtester_time_v04.c functions.c -lpthread -lrt -D_REENTRANT -o pack_gen
 * eg: sudo ./pack_gen path 1 1500 1000000 500 100 40 p5p1 p5p1 00:11:22:33:44:55
 * --------------------------------------------------
 */
