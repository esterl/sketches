#ifndef SKETCHES_RANDOM_H
#define SKETCHES_RANDOM_H

#include <stdlib.h> 
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

#endif
