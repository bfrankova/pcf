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

#ifndef _PACKET_TIME_INFO_H
#define _PACKET_TIME_INFO_H

#include <list>
#include <stdint.h>

#include "point2d.h"


/**
 * Time information about each packet
 */
class PacketTimeInfo {
  public:
    double time;
    uint32_t timestamp;
    point2d offset;
};

/**
 * Typedef for a list of all packets known for one computer.
 */
typedef std::list<PacketTimeInfo> packet_time_info_list;
typedef packet_time_info_list::iterator packet_iterator;

#endif
