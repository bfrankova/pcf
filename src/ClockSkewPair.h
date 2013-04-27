/* 
 * File:   ClockSkewPair.h
 * Author: bf
 *
 * Created on April 10, 2013, 11:58 AM
 */

#ifndef CLOCKSKEWPAIR_H
#define	CLOCKSKEWPAIR_H

class ClockSkewPair {
public:
    /// Clock skew pair (alpha, beta): y = alpha*x + beta
    double Alpha;
    double Beta;
    ClockSkewPair(){};
    ClockSkewPair(double a, double b){
        Alpha = a;
        Beta = b;
    };
};

#endif	/* CLOCKSKEWPAIR_H */

