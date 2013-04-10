/**
 * Copyright (C) 2012 Jakub Jirasek <xjiras02@stud.fit.vutbr.cz>
 *                    Libor Polčák <ipolcak@fit.vutbr.cz>
 * 
 * This file is part of pcf - PC fingerprinter.
 *
 * Pcf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pcf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pcf. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include <pcap.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>

#include "capture.h"
#include "ComputerInfoList.h"
#include "gnuplot_graph.h"
#include "Configurator.h"
#include "Debug.h"
#include "Tools.h"

/// Capture all packets on the wire
#define PROMISC 1

/// Pcap session handle
pcap_t *handle;

// IPv6 addr length (39B) + '\0' + some padding
#define ADDRESS_SIZE 64


void StopCapturing(int signum){
  pcap_breakloop(handle);
}

void GotPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
  // Allocate space for an address
  char address[ADDRESS_SIZE];
  // Ethernet header
  const struct ether_header *ether = (struct ether_header*) packet;
  // TCP header
  const struct tcphdr *tcp;
  // Packet arrival time (us)
  double arrival_time;
  // Timestamp
  uint32_t timestamp;
  
  // Sizes
  int size_ethernet;
  int size_ip;
  int size_tcp;
  
  /// Ethernet
  size_ethernet = sizeof(struct ether_header);
  u_int16_t ether_type = ntohs(ether->ether_type);
  
  /// IPv4
  if (ether_type == 0x0800) { 
    // IP header
    const struct ip *ip  = (struct ip*)(packet + size_ethernet);
    size_ip = sizeof(struct ip);
    /// Check if the packet is TCP
    if (ip->ip_p != IPPROTO_TCP)
      return;

    /// TCP
    tcp = (struct tcphdr*)(packet + size_ethernet + size_ip);
    if (inet_ntop(AF_INET, &(ip->ip_src), address, 64) == NULL)
    {
      fprintf(stderr, "Cannot get IP address\n");
      return;
    }
  }
  /// IPv6
  else if (ether_type == 0x86dd) { 
    // IP header
    const struct ip6_hdr *ip  = (struct ip6_hdr*)(packet + size_ethernet);
    size_ip = sizeof(struct ip6_hdr);
    /// Check if the packet is TCP
    if (ip->ip6_ctlun.ip6_un1.ip6_un1_nxt != IPPROTO_TCP)
      return;

    /// TCP
    tcp = (struct tcphdr*)(packet + size_ethernet + size_ip);
    if (inet_ntop(AF_INET6, &(ip->ip6_src), address, 64) == NULL)
    {
      fprintf(stderr, "Cannot get IP address\n");
      return;
    }
  }
  else
  {
      fprintf(stderr, "Unknown Ethernet type\n");
      return;
  }
  // skip TCP headers
  size_tcp = tcp->doff*4;
  if (size_tcp < 20) {
    if(debug)
      std::cerr << "Invalid TCP header length: " << size_tcp << " bytes" << std::endl;
    return;
  }
  
  // For sure, should filter pcap
  if (size_tcp == 20) {
    if(debug)
      std::cout << "TCP header without options" << std::endl;
    return;
  }
  /// TCP options
  u_char *tcp_options = (u_char *)(packet + size_ethernet + size_ip + sizeof(struct tcphdr));

  int options_size = size_tcp - 20;
  int options_offset = 0;
  
  // TCP options
  // +--------+--------+---------+--------+
  // |  Kind  | Length |       Data       |
  // +--------+--------+---------+--------+
  
  while (options_offset < options_size) {
    int kind = (int)tcp_options[options_offset];
    int option_len = 0;

    if (kind == 8) {
      timestamp = ntohl(*((uint32_t*) (&tcp_options[options_offset + 2])));
      /// Packet arrival time
      arrival_time = header->ts.tv_sec + (header->ts.tv_usec / 1000000.0);

      /// Save packet
      ComputerInfoList *computers = reinterpret_cast<ComputerInfoList*>(args);
      computers->new_packet(address, arrival_time, timestamp);
      return; // Packet processed
    }

    switch(kind) {
      /// EOL
      case 0:
        if(debug)
          std::cout << "TCP header option without timestamp" << std::endl;
        return;
      /// NOP
      case 1:
        options_offset++;
        break;
      default:
        option_len = (int)tcp_options[options_offset + 1];
        options_offset += option_len;
        break;
    }
  }
}


int StartCapturing() {
  // Dot notation network address
  char *net;
  // Dot notation network mask
  char *mask;
  // Error string
  char errbuf[PCAP_ERRBUF_SIZE];
  // Compiled filter expr.
  struct bpf_program fp;
  // Filter expr.
  std::string filter;
  // Netmask of sniffing device
  bpf_u_int32 maskp;
  // IP or sniffing device
  bpf_u_int32 netp;
  // Addresses
  struct in_addr addr;  
  
  /// Set the device
  char *dev = Configurator::instance()->dev;
  if (strcmp(dev, "") == 0) {
    dev = pcap_lookupdev(errbuf);
    if (dev == NULL) {
      fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
      return(2);
    }
  }
  
  if(debug)
      std::cout << "DEV: " << dev << std::endl;
  
  /// Find address and netmask for the device
  if (pcap_lookupnet(dev, &netp, &maskp, errbuf) == 0) {
    addr.s_addr = netp;
    net = inet_ntoa(addr);
    if (net == NULL)
        std::cerr << "Can't convert net address" << std::endl;
    else {
        if(debug)
            std::cout << "NET: " << net << std::endl;
    }
  
    /// Netmask
    addr.s_addr = maskp;
    mask = inet_ntoa(addr);
    if (mask == NULL)
        std::cerr << "Can't convert net mask" << std::endl;
    else {
        if(debug)
            std::cout << "MASK: " << mask << std::endl;
    }
  }
  else
    std::cerr << "(Can't get netmask for device: " <<  dev << std::endl;
  
  /// Open the device for sniffing
  handle = pcap_open_live(dev, BUFSIZ, PROMISC, 1000, errbuf);
  if (handle == NULL) {
    std::cerr << "Couldn't open device" <<  dev << ": " << errbuf << std::endl;
    return(2);
  }
  
  // TCP
  filter = "tcp";
  // TCP header with options and (very likely) with timestamps
  filter += " && ((tcp[12] >= 120) || (ip6[52] >= 120)) ";
 
  // Port
  if (Configurator::instance()->port != 0) {
    filter += " && port ";
    filter += Tools::IntToString(Configurator::instance()->port);
  }
  // Src
  if (strcmp(Configurator::instance()->src, "") != 0) {
    filter += " && src ";
    filter += Configurator::instance()->src;
  }
  // Dst
  if (strcmp(Configurator::instance()->dst, "") != 0) {
    filter += " && dst host ";
    filter += Configurator::instance()->dst;
  }
  // SYN
  if (Configurator::instance()->syn == 1) {
    filter += " && tcp[tcpflags] & tcp-syn == tcp-syn ";
  }
  // ACK	
  if (Configurator::instance()->ack == 1) {
    filter += " && tcp[tcpflags] & tcp-ack == tcp-ack ";
  }
  // Filter
  if (strcmp(Configurator::instance()->filter, "") != 0) {
      filter += " && ";
      filter += Configurator::instance()->filter;
  }
  
  if(debug)
      std::cout << "Filter: " << filter << std::endl;
  
  if (pcap_compile(handle, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
    std::cerr << "Couldn't parse filter " << filter << ": " << pcap_geterr(handle) << std::endl;
    return(2);
  }
  
  /// Apply the filter
  if (pcap_setfilter(handle, &fp) == -1) {
    std::cerr << "Couldn't install filter " << filter << ": " << pcap_geterr(handle) << std::endl;
    return(2);
  }
  
  /// Set alarm (if any)
  if (Configurator::instance()->time > 0) {
    signal(SIGALRM, StopCapturing);
    alarm(Configurator::instance()->time);
  }

  ComputerInfoList computers(Configurator::instance()->active, Configurator::instance()->database, Configurator::instance()->block, Configurator::instance()->timeLimit, Configurator::instance()->threshold);
  gnuplot_graph graph_creator;
  computers.AddObserver(&graph_creator);

  /// Set interrupt signal (ctrl-c or SIGTERM during capturing means stop capturing)
  struct sigaction sigact;
  memset(&sigact, 0, sizeof(sigact));
  sigact.sa_handler = StopCapturing;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = SA_SIGINFO;
  if (sigaction(SIGINT, &sigact, NULL) != 0) {
    std::cerr << "Couldn't set SIGINT" << std::endl;
    return(2);
  }
  if (sigaction(SIGTERM, &sigact, NULL) != 0) {
    std::cerr << "Couldn't set SIGTERM" << std::endl;
    return(2);
  }
  struct sigaction sigact2;
  memset(&sigact2, 0, sizeof(sigact2));
  sigact2.sa_handler = SIG_IGN;
  if (sigaction(SIGHUP, &sigact2, NULL) != 0) {
    std::cerr << "Couldn't set SIGHUP" << std::endl;
    return(2);
  }
  
  /// Print actual time
  time_t rawtime;
  time(&rawtime);
  std::cout << "Capturing started at: " << ctime(&rawtime) << std::endl;
  
  /// Start capturing TODO
  if (pcap_loop(handle, Configurator::instance()->number, GotPacket, reinterpret_cast<u_char*>(&computers)) == -1) {
    std::cerr << "An error occured during capturing: " << pcap_geterr(handle) << std::endl;
    return(2);
  }
  
  /// Close the session
  pcap_close(handle);
  
  return(0);
}
