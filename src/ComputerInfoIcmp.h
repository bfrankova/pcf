/* 
 * File:   ComputerInfoIcmp.h
 * Author: bf
 *
 * Created on April 14, 2013, 8:13 PM
 */

#ifndef COMPUTERINFOICMP_H
#define	COMPUTERINFOICMP_H

#include "ComputerInfo.h"
#include "ComputerInfoList.h"
#include <pthread.h>


class ComputerInfoIcmp : public ComputerInfo {
private:
    pthread_t pokingThread;
public:
    explicit ComputerInfoIcmp(ComputerInfoList * parent, const char * address) : ComputerInfo(parent, address){};
    void StartPoking();    
};

#endif	/* COMPUTERINFOICMP_H */

