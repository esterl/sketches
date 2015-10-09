#ifndef SKETCHES_RANDOM_H
#define SKETCHES_RANDOM_H

#include <stdlib.h> 
#include <random>
#include "ttmath/ttmath.h"

/******************** Random constructor by type ******************************/

template<typename T>
T random(){
    return (T) rand();
};

template<unsigned int length>
ttmath::UInt<length> random_ttmath(){
    ttmath::UInt<length> result;
    for ( unsigned int i = 0; i<length; i++)
        result.table[i] = rand();
    return result;
};

template<unsigned int length>
ttmath::UInt<length> random_ttmath(unsigned bits){
    ttmath::UInt<length> result;
    std::random_device rd;
    for ( unsigned int i = 0; i<length; i++){
        if ( bits < 64 ) {
            std::uniform_int_distribution<uint64_t> dist(0, (1UL<<bits) - 1);
            result.table[i] = dist(rd);
            bits = 0;
        } else {
            std::uniform_int_distribution<uint64_t> dist;
            result.table[i] = dist(rd);
            bits -= 64;
        }
    }
    return result;
};

template<typename T>
T random(unsigned bits);
#endif
