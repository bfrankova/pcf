/* 
 * File:   ComputerInfoIcmp.cc
 * Author: bf
 * 
 * Created on April 14, 2013, 8:13 PM
 */

#include "ComputerInfoIcmp.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

unsigned short in_cksum(unsigned short *addr, int len) {
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  // mop up an odd byte, if necessary
  if (nleft == 1) {
    *(unsigned char *) (&answer) = *(unsigned char *) w;
    sum += answer;
  }

  // add back carry outs from top 16 bits to low 16 bits
  // add hi 16 to low 16
  sum = (sum >> 16) + (sum & 0xffff);
  // add carry
  sum += (sum >> 16);
  // truncate to 16 bits
  answer = ~sum;
  return (answer);
}

void * sendIcmpRequests(void * arg){
  ComputerInfo * computer = static_cast<ComputerInfo*>(arg);
  int s;
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  
  const int dataLength = 40;
  
  struct iphdr *ip = (struct iphdr *) buffer;
  struct icmphdr *icmp = (struct icmphdr *) (ip + 1);
  struct sockaddr_in dst;
  int on = 1;

  // create RAW socket
  if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror("socket() error");
    exit(1);
  }

  // set socket options, tell the kernel we provide the IP structure
  if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0) {
    perror("setsockopt() for IP_HDRINCL error");
    exit(1);
  }
  // convert IP
  dst.sin_family = AF_INET;
  if (inet_pton(AF_INET, computer->address.c_str(), &(dst.sin_addr)) != 1) {
    perror("could not convert IP");
  }
  // create IP header
  ip->ihl = 5;
  ip->version = 4;
  ip->tos = 0;
  ip->tot_len = dataLength;
  ip->id = htons(0);
  ip->frag_off = 0;
  ip->ttl = 64;
  ip->protocol = IPPROTO_ICMP;
  ip->daddr = dst.sin_addr.s_addr;
  ip->check = 0;

  // create ICMP header
  icmp->type = ICMP_TSTAMP;
  icmp->code = 0;
  
  while (1) {
    icmp->checksum = 0;
    icmp->checksum = in_cksum((unsigned short *) icmp, sizeof (struct icmphdr));
    // send packet
    if (sendto(s, buffer, dataLength, 0, (struct sockaddr *) &dst, sizeof(struct sockaddr)) < 0)
      perror("sendto failed\n");
    
    sleep(3);
  }
}

void ComputerInfoIcmp::StartPoking(){
  pthread_create(&pokingThread, NULL, sendIcmpRequests , this);
}
