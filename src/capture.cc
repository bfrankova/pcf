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
#include <sstream>
#include <string>
#include <regex>

#include <pcap.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <pthread.h>

#include "capture.h"
#include "ComputerInfoList.h"
#include "gnuplot_graph.h"
#include "Configurator.h"
#include "Tools.h"
#include "ComputerInfoIcmp.h"

/// Capture all packets on the wire
#define PROMISC 1

/// Pcap session handle
pcap_t *handle;

// IPv6 addr length (39B) + '\0' + some padding
#define ADDRESS_SIZE 64

ComputerInfoList * computersTcp;
ComputerInfoList * computersIcmp;
ComputerInfoList * computersJavascript;

void StopCapturing(int signum){
  pcap_breakloop(handle);
}

void GotPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
  // Allocate space for an address
  char address[ADDRESS_SIZE];
  // Ethernet header
  const struct ether_header *ether = (struct ether_header*) packet;
  // TCP header
  const struct tcphdr *tcp = NULL;
  // ICMP header
  const struct icmphdr *icmp = NULL;
  std::string httpRequest;
  // Packet arrival time (us)
  double arrival_time;
  // Timestamp
  uint32_t timestamp;
  int pokeOk = true;
  //
  std::string type = "";
  
  // number of processed packets
  static int n_packets = 0;
  
  // Sizes
  int size_ethernet;
  int size_ip;
  int size_tcp;
  bool newIp = false;
  
  /// Ethernet
  size_ethernet = sizeof(struct ether_header);
  u_int16_t ether_type = ntohs(ether->ether_type);
  
  /// IPv4
  if (ether_type == 0x0800) { 
    // IP header
    const struct ip *ip  = (struct ip*)(packet + size_ethernet);
    size_ip = sizeof(struct ip);
    // Check if the packet is ICMP or TCP
    if(ip->ip_p == IPPROTO_ICMP){
      type = "icmp";
      icmp = (struct icmphdr*)(packet + size_ethernet + size_ip);
    }
    else if (ip->ip_p == IPPROTO_TCP){
      type = "tcp";
      tcp = (struct tcphdr*)(packet + size_ethernet + size_ip);
    }
    else
      return;
    
    // get IP address
    if (inet_ntop(AF_INET, &(ip->ip_src), address, 64) == NULL)
    {
      fprintf(stderr, "Cannot get IP address\n");
      return;
    }
  }
  /// IPv6
  else if (ether_type == 0x86dd) { 
    // IP header
    pokeOk = false;
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
    // replace colons with dashes (because of Win filenames conventions)
    std::string replaceColon = "-";
    std::string findColon = ":";
    std::string address6 = (std::string) address;
    for(std::string::size_type i = 0; (i = address6.find(findColon, i)) != std::string::npos;)
    {
        address6.replace(i, findColon.length(), replaceColon);
        i += replaceColon.length() - findColon.length() + 1;
    }
    strcpy(address, address6.c_str());
  }
  else
  {
      fprintf(stderr, "Unknown Ethernet type\n");
      return;
  }
  
  if(type == "tcp"){
    // skip TCP headers
  size_tcp = tcp->doff*4;
  if (size_tcp < 20) {
    if(Configurator::instance()->debug)
      std::cerr << "Invalid TCP header length: " << size_tcp << " bytes" << std::endl;
    return;
  }
  
  // For sure, should filter pcap
  if (size_tcp == 20) {
    if(Configurator::instance()->debug)
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
      /// Packet arrival timearrival_time
      arrival_time = header->ts.tv_sec + (header->ts.tv_usec / 1000000.0);

      /// Save packet
      n_packets++;
      newIp = !computersTcp->new_packet(address, arrival_time, timestamp);
      if(newIp){
        if(pokeOk && !Configurator::instance()->icmpDisable)
          computersIcmp->to_poke_or_not_to_poke(address);
      }
      
      // TODO: remove temporary return
      // return;
      
      int remaining_length = header->len - (size_ethernet + size_ip + size_tcp);
      // TCP without HTTP
      if(remaining_length == 0)
          return;
      
      // parse HTTP
      char * hl = (char*)tcp + size_tcp;
      std::string httpRequest = "";
      httpRequest.append(hl, remaining_length);

      // find timestamp
      std::string ts = "";
      int found = httpRequest.find("ts=");   
      if(found < 0)
          return;
      // check if enough data to read
      if(found+16 > (int)httpRequest.length())
          return;
      ts = httpRequest.substr(found+3, 13);
      // convert timestamp
      long long longTimestamp = atoll(ts.c_str());
      // convert UNIX timestamp to h:m:s:ms
      longTimestamp = longTimestamp % (3600 * 24 * 1000);
      // save new packet
      computersJavascript->new_packet(address, arrival_time, longTimestamp);
      return; // Packet processed
    }
    
    switch(kind) {
      /// EOL
      case 0:
        if(Configurator::instance()->debug)
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
  // parse HTTP header
  
  
  }
  else if(type == "icmp"){
    // ICMP header
    // +--------+--------+---------+--------+
    // |  Type  |  Code  |  Header Chcksum  |
    // +--------+--------+---------+--------+
    // |    Identifier   |   Sequence Num   |
    // +--------+--------+---------+--------+
    // |        Originate Timestamp         |
    // +--------+--------+---------+--------+
    // |         Recieve Timestamp          |
    // +--------+--------+---------+--------+
    // |        Transmit Timestamp          |
    // +--------+--------+---------+--------+
      
      // packet is not ICMP timestamp reply -> throw away
      if(icmp->type != ICMP_TSTAMPREPLY)
          return;
      // retrieve appropriate timestamp
      unsigned int * newTimestamp = (unsigned int *)icmp + 3;
      timestamp = (uint32_t) ntohl(*newTimestamp);
      arrival_time = header->ts.tv_sec + (header->ts.tv_usec / 1000000.0);
      // save packet 
      computersIcmp->new_packet(address, arrival_time, timestamp);
      // packet processed
      return;
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
  
  if(Configurator::instance()->debug)
      std::cout << "DEV: " << dev << std::endl;
  
  /// Find address and netmask for the device
  if (pcap_lookupnet(dev, &netp, &maskp, errbuf) == 0) {
    addr.s_addr = netp;
    net = inet_ntoa(addr);
    if (net == NULL)
        std::cerr << "Can't convert net address" << std::endl;
    else {
        if(Configurator::instance()->debug)
            std::cout << "NET: " << net << std::endl;
    }
  
    /// Netmask
    addr.s_addr = maskp;
    mask = inet_ntoa(addr);
    if (mask == NULL)
        std::cerr << "Can't convert net mask" << std::endl;
    else {
        if(Configurator::instance()->debug)
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
  filter = "(tcp";
  // TCP header with options and (very likely) with timestamps
  filter += " && ((tcp[12] >= 120) || (ip6[52] >= 120)) ";
 
  // Port
  if (Configurator::instance()->port != 0) {
    filter += " && port "  + Tools::IntToString(Configurator::instance()->port);
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
      filter += " && (";
      filter += Configurator::instance()->filter;
      filter += ")";
  }
  
  filter += ") || (icmp && icmp[icmptype] == icmp-tstampreply)";
  
  if(Configurator::instance()->debug)
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

  computersTcp = new ComputerInfoList("tcp");
  computersJavascript = new ComputerInfoList("javascript");
  computersIcmp = new ComputerInfoList("icmp");
  // ComputerInfoList computers(Configurator::instance()->active, Configurator::instance()->database, Configurator::instance()->block, Configurator::instance()->timeLimit, Configurator::instance()->threshold);
  gnuplot_graph graph_creator_tcp("tcp");
  gnuplot_graph graph_creator_javascript("javascript");
  gnuplot_graph graph_creator_icmp("icmp");
  computersTcp->AddObserver(&graph_creator_tcp);
  computersIcmp->AddObserver(&graph_creator_icmp);
  computersJavascript->AddObserver(&graph_creator_javascript);

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
  if (pcap_loop(handle, Configurator::instance()->number, GotPacket, NULL) == -1) {
    std::cerr << "An error occured during capturing: " << pcap_geterr(handle) << std::endl;
    return(2);
  }
  
  /// Close the session
  pcap_close(handle);
  return(0);
}
