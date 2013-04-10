/**
 * Copyright (C) 2009-2012 Libor Polčák <ipolcak@fit.vutbr.cz>
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

#ifndef _COMPUTER_INFO_H
#define _COMPUTER_INFO_H

#include <list>
#include <string>
#include <utility>

#include "TimeSegment.h"
#include "PacketTimeInfo.h"
#include "TimeSegmentList.h"
#include "ClockSkewPair.h"
#include "PacketSegment.h"



/**
 * All informations known about each computer including time information about all received packets.
 */
class ComputerInfo {

  // Private types
  private:
    /// List of time informations about packets
    packetTimeInfoList packets;

    /// Frequency of the computer
    int freq;

    /// Time of the last added packet
    double lastPacketTime;

    /// Time of the packet with last confirmed skew
    double lastConfirmedPacketTime;

    /// Last confirmed skew
    ClockSkewPair confirmedSkew;

    /// Time of the start of the capture
    double startTime;

    /// Skew information about one computer
    std::list<PacketSegment> packetSkewList;

  // Constructors
  public:
    ComputerInfo(double first_packet_delivered, uint32_t first_packet_timstamp,
        const char* its_address);

  // Destructor
  public:
    ~ComputerInfo() {}

  // Getters
  public:
    const std::string& get_address() const
    {
      return address;
    }
    
    /// Address of the computer
    std::string address;

    int get_freq() const
    {
      return freq;
    }

    unsigned long get_packets_count() const
    {
      return packets.size();
    }

    double get_last_packet_time() const
    {
      return lastPacketTime;
    }

    double get_start_time() const
    {
      return startTime;
    }

    uint32_t get_last_packet_timestamp() const
    {
      return packets.rbegin()->Timestamp;
    }

    const PacketSegment& get_skew() const
    {
      return *(packetSkewList.rbegin());
    }
    
    TimeSegmentList timeSkewList;

  // Functions manipulating the state
  public:
    //
    TimeSegmentList LastCalculatedSkew;
      
    /**
     * Adds a new packet without updating or recomputing anything
     * @param[in] packet_delivered Arrival time of the new packet
     * @param[in] timestamp TCP timestamp of the new packet
     * @param[inout] skews Storage of clock skews of known devices
     */
    void insert_packet2(double packet_delivered, uint32_t timestamp);

    /**
     * Adds a new packet and possibly recomputes related informations
     * @param[in] packet_delivered Arrival time of the new packet
     * @param[in] timestamp TCP timestamp of the new packet
     * @param[inout] skews Storage of clock skews of known devices
     */
    void insert_packet(double packet_delivered, uint32_t timestamp);

    /**
     * Restart measurement
     * @param[in] packet_delivered Arrival time of the first packet
     * @param[in] timestamp TCP timestamp of the first packet
     */
    void restart(double packet_delivered, uint32_t timestamp);
    /// Reduces unnecessary information about packets
    void reduce_packets(packet_iterator start, packet_iterator end);

  private:
    ///Performs actions after a block of packets is captured
    void block_finished(double packet_delivered);
    /// Adds initialized empty skew information
    void add_empty_skew(packetTimeInfoList::iterator start);
    /// Computes a new skew
    ClockSkewPair compute_skew(const packet_iterator &start, const packet_iterator &end);
    /// Computes a new frequency
    int compute_freq();
    /** 
     * Save packets into file (called 'IP address.log')
     * @param[in] rewrite Boolean that conrols if the file is overwritten or the packets are appended
     * @return 0 if ok
     * */
    int save_packets(short int rewrite);

};

#endif
