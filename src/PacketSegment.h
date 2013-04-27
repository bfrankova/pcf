/* 
 * File:   PacketSegment.h
 * Author: bf
 *
 * Created on April 7, 2013, 5:48 PM
 */

#ifndef PACKETSEGMENT_H
#define	PACKETSEGMENT_H
#include "PacketTimeInfo.h"

class PacketSegment {
    /// Structure describing clock skew
public:
      double alpha;
      double beta;
      packet_iterator first;
      packet_iterator confirmed;
      packet_iterator last;
};


#endif	/* PACKETSEGMENT_H */

