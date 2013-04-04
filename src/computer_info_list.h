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

#ifndef _COMPUTER_INFO_LIST_H
#define _COMPUTER_INFO_LIST_H

#include "computer_info.h"
#include "clock_skew_guard.h"
#include "observer.h"

class computer_info;

/**
 * All informations known about a set of computers.
 */
class computer_info_list : public observable<const computer_skew> {
  // Attributes
  private:
    /// Informations about packet timing
    std::list<computer_info *> computers;
    /// Informations about clock skew
    // clock_skew_guard skews;
    /// Last time when inactive computers were detected
    double last_inactive;

    /// Filename of DB with saved computers
    const char *saved_computers;

    // Program configuration
    // Filenames of databases
    char *active;

    /// Number of packets in one block
    const int block;
    /**
     * Number of seconds of inactivity afeter which old data are erased
     * and the tracking is restarted
     */
    const int TIME_LIMIT;
    
    /**
     * Number of PPM that controls if more addresses are treated as if they
     * belong to the same computer.
     */
    const double THRESHOLD;
    
    void construct_notify(const std::string &ip, const identity_container &identitites, const skew &s) const;
    
    skew * getSkew(std::string ip);

  // Constructors
  public:
    computer_info_list(char *_active, char *saved_computers_db, int _block, int _time_limit, double _threshold):
      last_inactive(time(NULL)), active(_active), block(_block), TIME_LIMIT(_time_limit), THRESHOLD(_threshold)
    {}

  // Destructor
  public:
    ~computer_info_list();

  // Public methods
  public:
    /**
     * New packet processing (classify, save, compute...)
     * @param[in] address IP address of the source
     * @param[in] time Real time when packet arrived
     * @param[in] timestamp PCAP timestamp of the packet
     * @return 0 if ok
     */
    void new_packet(const char *address, double time, uint32_t timestamp);

    /**
     * Registers a new observer for clock changes
     *
     * @param[in] obs The observer to be added
     */
    /*void add_observer(observer<const computer_skew>* obs)
    {
      this->add_observer(obs);
    }*/
    
    /**
     * Adds or updates clock skew value of a address
     * @param[in] ip The IP address for which the clock skew is provided
     * @param[in] skew Clock skew of the IP address
     */
    void update_skew(const std::string &ip, const skew &s);

    /**
     * Returns IP addresses with similar clock skew to the IP address provided
     * @param[in] ip The IP whose clock skew will be compared
     */
    const identity_container get_similar_identities(const std::string &ip);

    /// Returns THRESHOLD
    double get_threshold() {
      return THRESHOLD;
    }

};

#endif
