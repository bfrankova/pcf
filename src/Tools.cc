/* 
 * File:   Tools.cc
 * Author: bf
 * 
 * Created on April 10, 2013, 5:43 PM
 */

#include "Tools.h"
#include <sstream>

std::string Tools::IntToString(int number){
    std::stringstream ss;
    ss << number;
    return ss.str();
};

