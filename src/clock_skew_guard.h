/**
 * Copyright (C) 2012 Libor Polčák <ipolcak@fit.vutbr.cz>
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

#ifndef _CLOCK_SKEW_GUARD_H
#define _CLOCK_SKEW_GUARD_H

#include <map>
#include <string>

#include "clock_skew.h"
#include "computer_skew.h"
#include "observer.h"
#include "skew.h"

/**
 * This class stores information about current known clock skew values
 */
class clock_skew_guard: public observable<const computer_skew>
{
  // Attributes
  private:
    

  // Constructors, destructors
  public:
    /**
     * Constructor
     *
     * @param[in] threshold Number of PPM that means that the address belongs to the same computer
     */
    //clock_skew_guard(const double threshold, const char* saved_computers_db): THRESHOLD(threshold), saved_computers(saved_computers_db) {}

    //~clock_skew_guard() {}

  // Private methods
  private:
    

  // Public methods
  public:
    
};

#endif
