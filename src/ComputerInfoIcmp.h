/**
 * Copyright (C) 2013 Barbora Frankova <xfrank08@stud.fit.vutbr.cz>
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

#endif
