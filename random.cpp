#include "random.h"

template<>
ttmath::UInt<2> random<ttmath::UInt<2> >(){
    return random_ttmath<2>();
}

template<>
ttmath::UInt<3> random<ttmath::UInt<3> >(){
    return random_ttmath<3>();
}

template<>
ttmath::UInt<17> random<ttmath::UInt<17> >(){
    return random_ttmath<17>();
}

template<>
unsigned int random<unsigned int>(){
    return (unsigned int) rand();
}

template<>
ttmath::UInt<2> random<ttmath::UInt<2> >(unsigned bits){
    return random_ttmath<2>(bits);
}

template<>
ttmath::UInt<3> random<ttmath::UInt<3> >(unsigned bits){
    return random_ttmath<3>(bits);
}

template<>
ttmath::UInt<17> random<ttmath::UInt<17> >(unsigned bits){
    return random_ttmath<17>(bits);
}

template<>
uint32_t random<uint32_t>(unsigned bits){
    std::random_device rd;
    std::uniform_int_distribution<uint32_t> dist(0, (1UL<<bits) -1);
    return dist(rd);
}

template<>
uint64_t random<uint64_t>(unsigned bits){
    std::random_device rd;
    std::uniform_int_distribution<uint64_t> dist(0, (1UL<<bits) -1);
    return dist(rd);
}

