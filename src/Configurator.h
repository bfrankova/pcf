/**
 * Copyright (C) 2012 Jakub Jirasek <xjiras02@stud.fit.vutbr.cz>
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

#ifndef _CONFIGURATOR_H
#define _CONFIGURATOR_H


/**
 * Structure with all config data
 */

class Configurator {
private:
    static Configurator * innerInstance;
    
public:
    Configurator() {};
    
  char dev[10];
  int number;
  unsigned int time;
  int port;
  char src[16];
  char dst[16];
  short syn;
  short ack;
  char filter[4096];
  
  char active[1024];
  char database[1024];
  
  int block;
  int time_limit;
  double threshold;
  
  
  void init();

  /**
   * Fill the config structure
   * @param[in] filename          Config file name
   */
  void get_config(const char *filename);
  
  static Configurator * instance();
};


#endif
