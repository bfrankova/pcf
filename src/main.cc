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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "capture.h"
#include "check_computers.h"
#include "Configurator.h"
#include "Debug.h"

int debug = false;

/**
 * Print banner
 */
void print_banner()
{
  printf("pcf  Copyright (C) 2012 Jakub Jirasek <xjiras02@stud.fit.vutbr.cz>\n\n"
         "This program comes with ABSOLUTELY NO WARRANTY.\n"
         "This is free software, and you are welcome to redistribute it\n"
         "under certain conditions.\n\n");
}

/**
 * Print help
 */
void print_help()
{
  printf("Usage: pcf [Options] [Interface]\n\n"
         "  -h\t\tPrint this help\n"
         "  -n\t\tNumber of packets to capture (0 for infinity)\n"
         "  -t\t\tTime for capturing (in seconds, 0 for infinity)\n"
         "  -p\t\tPort number (1-65535)\n\n"
         "Examples:\n"
         "  pcf\n"
         "  pcf -n 100 -t 600 -p 80 wlan0\n\n");
}

/**
 * Main
 */

int main(int argc, char *argv[])
{
  /// Print banner
  print_banner();
  
  /// Checking permissions (must be root)
  if (getuid()) {
    fprintf(stderr, "Must have root permissions to run this program!\n");
    return(2);
  }
  
  /// Get config
  char filename[] = "config";
  Configurator::instance()->GetConfig(filename);
  
  /// Get params
  int c;
  opterr = 0;
  while ((c = getopt(argc, argv, "dhn:t:p:")) != -1) {
    switch(c) {
      case 'd':
        debug = true;
        break;
      case 'h':
        print_help();
        return(0);
      case 'n':
        if (atoi(optarg))
          Configurator::instance()->number = atoi(optarg);
        else
          fprintf(stderr, "Wrong number of packets\n");
        break;
      case 't':
        if (atoi(optarg))
          Configurator::instance()->time = atoi(optarg);
        else
          fprintf(stderr, "Wrong time for capturing\n");
        break;
      case 'p':
        if (atoi(optarg) && (atoi(optarg) < 65535))
          Configurator::instance()->port = atoi(optarg);
        else
          fprintf(stderr, "Wrong port number\n");
        break;
    }
  }
  c = optind;
  if (c < argc) {
    if (strlen(argv[c]) < 10)
      strcpy(Configurator::instance()->dev, argv[c]);
    else {
      fprintf(stderr, "Device name too long (%s)\nSetting to any", argv[c]);
      strcpy(Configurator::instance()->dev, "any");
    }
  }
  
  /// Get packets
  if (StartCapturing() != 0)
    return(2);
  
  return(0);
}
