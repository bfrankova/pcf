#
# Copyright (C) 2012 Jakub Jirasek <xjiras02@stud.fit.vutbr.cz>
#                    Barbora Frankova <xfrank08@stud.fit.vutbr.cz>
# 
# This file is part of pcf - PC fingerprinter.
#
# Pcf is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Pcf is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with pcf. If not, see <http://www.gnu.org/licenses/>.
#

program = pcf

OBJ = capture.o main.o ComputerInfoList.o Configurator.o Computations.o check_computers.o ComputerInfo.o ComputerInfoIcmp.o gnuplot_graph.o TimeSegmentList.o Tools.o
LOG_READER_OBJ = log_reader.o ComputerInfoList.o Configurator.o Computations.o check_computers.o ComputerInfo.o ComputerInfoIcmp.o gnuplot_graph.o TimeSegmentList.o
HEAD = capture.h ComputerInfoList.h ClockSkewPair.h Configurator.h Computations.h check_computers.h ComputerInfo.h ComputerInfoIcmp.h PacketTimeInfo.h Point.h Observer.h Observable.h TimeSegment.h AnalysisInfo.h gnuplot_graph.h TimeSegmentList.h Tools.h
OPT = -pthread -lpcap -lm `xml2-config --cflags --libs`
CC = g++
DEFINE ?= 
CXXFLAGS ?= -O2 -Wall -std=c++0x $(DEFINE)
LDFLAGS ?=
LD = g++

.PHONY: profiling uninstall clean 

all: $(program) log_reader

profiling:
	make clean
	make CXXFLAGS="-pg -Wall -std=c++0x $(DEFINE)" LDFLAGS="-pg"

install: all
	cp -r pcf config log_reader coupler.py match_finder.sh bulk_log_reader.sh ../bin
	cp -r www ../bin
	mkdir -p ../bin/graph ../bin/log 
	mkdir -p ../bin/graph/icmp ../bin/log/icmp
	mkdir -p ../bin/graph/javascript ../bin/log/javascript

uninstall:
	rm -f ../bin/pcf ../bin/log_reader ../bin/*.sh ../bin/*.py

clean:
	rm -f *.o *~ $(program) log_reader

$(program): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $(program) $(OPT)

log_reader: $(LOG_READER_OBJ)
	$(LD) $(LOG_READER_OBJ) $(LDFLAGS) -o $@ $(OPT)

$(OBJ): $(HEAD)

$(LOG_READER_OBJ): $(HEAD)

check_computers.o: check_computers.cc check_computers.h
	$(CC) $(CXXFLAGS) -c `xml2-config --cflags --libs` check_computers.cc
