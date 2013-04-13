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

#include <cmath>
#include <iostream>

#include "ComputerInfoList.h"
#include "check_computers.h"
#include "Configurator.h"


ComputerInfoList::~ComputerInfoList() {}

void ComputerInfoList::to_poke_or_not_to_poke(std::string address){
  for (std::list<ComputerInfo *>::iterator it = computers.begin(); it != computers.end(); ++it) {
    if ((*it)->get_address() == address)
      return;
  }
  ComputerInfo *new_computer = new ComputerInfo(this, address.c_str());
  // TODO: decouple new thread to poke the computer under given address (e.g., new_computer->start_poking())
  computers.push_back(new_computer);
  save_active(computers, Configurator::instance()->active, *this);
}

void ComputerInfoList::new_packet(const char *address, double ttime, uint32_t timestamp)
{
  static unsigned long total = 0;
  printf("\r%lu packets captured", ++total);
  fflush(stdout);

  bool found = false;
  for (std::list<ComputerInfo *>::iterator it = computers.begin(); it != computers.end(); ++it) {
    if ((*it)->get_address() != address) {
      continue;
    }
    found = true;

    // Computer already known
    ComputerInfo &known_computer = **it;
    
    // first received packet for this IP (ICMP)
    if(!known_computer.firstPacketReceived){
        known_computer.insert_first_packet(ttime, timestamp);
        break;
    }
        
    /// Too much time since last packet so start from the beginning
    if ((ttime - known_computer.get_last_packet_time()) > Configurator::instance()->timeLimit) {
      known_computer.restart(ttime, timestamp);
#ifdef DEBUG
      fprintf(stderr, "%s timeout: starting a new tracking\n", known_computer.get_address().c_str());
#endif
      break;
    }

    // Check if packet has the same or lower timestamp
    if (timestamp <= known_computer.get_last_packet_timestamp()) {
#ifdef DEBUG
      if (timestamp < known_computer.get_last_packet_timestamp())
        fprintf(stderr, "%s: Lower timestamp %u %u\n", known_computer.get_address().c_str(), timestamp, known_computer.get_last_packet_timestamp());
#endif
      break;
    }

    // Stop supporting lists with stupid frequency
    if (std::fabs(known_computer.get_freq()) > 10000) {
#ifdef DEBUG
      fprintf(stderr, "%s: too high frequency of %d\n", known_computer.get_address().c_str(), known_computer.get_freq());
#endif
      known_computer.restart(ttime, timestamp);
      break;
    }

    // Insert packet
    known_computer.insert_packet(ttime, timestamp);
    known_computer.check_block_finish(ttime);
    update_skew(address, known_computer.NewTimeSegmentList);

#if 0
    std::cerr << known_computer.get_address() << ": " << known_computer.get_packet_count() << std::endl;
#endif
  }

  if (!found) {
    ComputerInfo *new_computer = new ComputerInfo(this, address);
    new_computer->insert_first_packet(ttime, timestamp);
    computers.push_back(new_computer);
    save_active(computers, Configurator::instance()->active, *this);
  }

  if (ttime > (last_inactive + Configurator::instance()->timeLimit / 4)) {
    /// Save active computers
    for (std::list<ComputerInfo *>::iterator it = computers.begin(); it != computers.end(); ++it) {
      if (ttime - (*it)->get_last_packet_time() > Configurator::instance()->timeLimit) {
        delete(*it);
        it = computers.erase(it);
      }
    }

    save_active(computers, Configurator::instance()->active, *this);
    last_inactive = ttime;
  }
}

void ComputerInfoList::construct_notify(const std::string &ip, const identity_container &identitites, const TimeSegmentList &s) const
{
  AnalysisInfo cs = {ip, identitites, s};
  Notify(cs);
}


TimeSegmentList * ComputerInfoList::getSkew(std::string ip)
{
    for(std::list<ComputerInfo *>::iterator it = computers.begin(); it != computers.end(); ++it)
    {
        if( (*it)->address == ip )
        {
            return &((*it)->timeSegmentList);
        }
    }
    return NULL;
}


void ComputerInfoList::update_skew(const std::string &ip, const TimeSegmentList &s) {
  identity_container old_identities = get_similar_identities(ip);

  // Update database, be it a new address or an update
  TimeSegmentList * target_skew = getSkew(ip);
  //
  if(target_skew == NULL) 
  {
      std::cerr << "Pseudo-exception: entry should be present in computer list, but is not. Ip=" << ip << std::endl;
      exit(1);
  }
  //
  *(getSkew(ip)) = s;
  
  // Notify observers
  identity_container new_identities = get_similar_identities(ip);
  construct_notify(ip, new_identities, s);

  for (identity_container::iterator it = old_identities.begin(); it != old_identities.end(); ++it) {
    if (new_identities.find(*it) == new_identities.end()) {
      construct_notify(*it, get_similar_identities(*it), *(getSkew(ip)));
    }
  }

  for (identity_container::iterator it = new_identities.begin(); it != new_identities.end(); ++it) {
    if (old_identities.find(*it) == old_identities.end()) {
      construct_notify(*it, get_similar_identities(*it), *(getSkew(ip)));
    }
  }
}

const identity_container ComputerInfoList::get_similar_identities(const std::string &ip)
{
  identity_container identities;

  TimeSegmentList * reference_skew = getSkew(ip);
  if (reference_skew == NULL) {
    // Given address is not known
    return identities;
  }

  if (reference_skew->is_constant()) {
    find_computer_in_saved(reference_skew->get_last_alpha(), identities, Configurator::instance()->threshold, Configurator::instance()->database);
  }
  
  for(std::list<ComputerInfo *>::iterator it = computers.begin(); it != computers.end(); ++it)
    {
        if( (*it)->address == ip )
        {
            continue;
        }
        
        if (reference_skew->is_similar_with((*it)->timeSegmentList, Configurator::instance()->threshold)) {
        identities.insert((*it)->address);
    }
  }

  return identities;
}