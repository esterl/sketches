#ifndef SKETCHES_RANDOM_H
#define SKETCHES_RANDOM_H

#include <stdlib.h> 

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

template<>
ttmath::UInt<2> random<ttmath::UInt<2> >(){
    return random_ttmath<2>();
}

template<>
ttmath::UInt<132> random<ttmath::UInt<132> >(){
    return random_ttmath<132>();
}

template<>
unsigned int random<unsigned int>(){
    return (unsigned int) rand();
};
#endif
