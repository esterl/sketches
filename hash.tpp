#include "mersenne.h"
#include "random.h"

inline bool isPowerOfTwo(unsigned x)
{
  return ((x != 0) && !(x & (x - 1)));
}
/**************************Hash_CW2 implementation*****************************/

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned B, T2 seed0, T2 seed1)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    seeds[0] = mersenne_modulus<T1>(seed0, mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(seed1, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::~Hash_CW2()
{
}

template<typename T1, typename T2>
unsigned Hash_CW2<T1,T2>::element(T1 j)
{
    T2 result = j;
    result = mersenne_modulus(result*seeds[1], mersenne_exponent) + seeds[0];
    result = mersenne_modulus(result, mersenne_exponent);
    T2 temp = mask;
    unsigned res = result & temp;
    return res;
}

template<typename T1, typename T2>
Hash<T1>* Hash_CW2<T1,T2>::copy()
{
    Hash_CW2<T1,T2> *result = new Hash_CW2<T1,T2>(mask+1, seeds[0], seeds[1]);
    return (Hash<T1>*) result;
}


/**************************Hash_CW4 implementation*****************************/

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned B, T2 seed0, T2 seed1, T2 seed2, T2 seed3)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    seeds[0] = mersenne_modulus<T1>(seed0, mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(seed1, mersenne_exponent);
    seeds[2] = mersenne_modulus<T1>(seed2, mersenne_exponent);
    seeds[3] = mersenne_modulus<T1>(seed3, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::~Hash_CW4()
{
}

template<typename T1, typename T2>
unsigned Hash_CW4<T1,T2>::element(T1 j)
{
    T2 result;
    T2 exp1 = j;
    T2 exp2 = mersenne_modulus<T2>(exp1*exp1, mersenne_exponent);
    T2 exp3 = mersenne_modulus<T2>(exp2*exp1, mersenne_exponent);
    result = mersenne_modulus<T2>(exp3*seeds[3], mersenne_exponent) +
                mersenne_modulus<T2>(exp2*seeds[2], mersenne_exponent);
    result = mersenne_modulus<T2>(result, mersenne_exponent) +
                mersenne_modulus<T2>(exp1*seeds[1], mersenne_exponent);
    result = mersenne_modulus<T2>(result, mersenne_exponent) + seeds[0];
    result = mersenne_modulus<T2>(result, mersenne_exponent);
    T2 temp = mask;
    unsigned res = result & temp;
    return res;
}

template<typename T1, typename T2>
Hash<T1>* Hash_CW4<T1,T2>::copy()
{
    Hash_CW4<T1,T2> * result = new Hash_CW4<T1,T2>( mask+1, seeds[0], seeds[1], 
                                                    seeds[2], seeds[3]);
    return (Hash<T1>*) result;
}

/**************************Hash_Tab implementation*****************************/
const uint32_t POW8 = 256;      // 2^8
const uint32_t POW16 = 65536;   // 2^16
const uint32_t POW19 = 524288;  // 2^19
const uint32_t POW20 = 1048576; // 2^20
const uint32_t POW21 = 2097152; // 2^21
const uint64_t mask16 = 0xFFFF;
const uint64_t mask21 = 0x1FFFFF;

// Implementation for 8b --> Mersenne prime 13 == 8191 (max number of buckets)
Hash_Tab<uint8_t>::Hash_Tab(unsigned B, prime13_t seed0, prime13_t seed1, 
                            prime13_t seed2, prime13_t seed3)
{
    table = new uint16_t[POW8];
    
    // Use Hash_CW4 to fill the table:
    Hash_CW4<uint8_t, prime13_t> aux_hash = Hash_CW4<uint8_t, prime13_t>(B, seed0, seed1, seed2, seed3);
    for (unsigned i=0; i < POW8; i++)
        table[i] = aux_hash.element(i);
}

// Empty constructor (for copy)
Hash_Tab<uint8_t>::Hash_Tab()
{
    table = new uint16_t[POW8];
}

unsigned Hash_Tab<uint8_t>::element(uint8_t j)
{
    return table[j];
}

Hash<uint8_t>* Hash_Tab<uint8_t>::copy()
{
    Hash_Tab<uint8_t> *result = new Hash_Tab<uint8_t>();
    
    for (unsigned i=0; i < POW8; i++)
        result->table[i] = table[i];
    return (Hash<uint8_t>*) result;
}

Hash_Tab<uint8_t>::~Hash_Tab()
{
    delete[] table;
}


// Implementation for 16b
Hash_Tab<uint16_t>::Hash_Tab(unsigned B, prime17_t seed0, prime17_t seed1, 
                                prime17_t seed2,prime17_t seed3)
{
    table = new uint16_t[POW16];
    
    // Use Hash_CW4 to fill the table:
    Hash_CW4<uint16_t, prime17_t> aux_hash = Hash_CW4<uint16_t, prime17_t>(B, seed0, seed1, seed2, seed3);
    for (unsigned i=0; i < POW16; i++)
        table[i] = aux_hash.element(i);
}

// Empty constructor (for copy)
Hash_Tab<uint16_t>::Hash_Tab()
{
    table = new uint16_t[POW16];
}

unsigned Hash_Tab<uint16_t>::element(uint16_t j){
    return table[j];
}

Hash<uint16_t>* Hash_Tab<uint16_t>::copy(){
    Hash_Tab<uint16_t> *result = new Hash_Tab<uint16_t>();
    
    for (unsigned i=0; i < POW16; i++){
        result->table[i] = table[i];
    }
    return (Hash<uint16_t>*) result;
}

Hash_Tab<uint16_t>::~Hash_Tab()
{
    delete[] table;
}

// Implementation for 32b
Hash_Tab<uint32_t>::Hash_Tab(unsigned B, prime31_t* s0, prime31_t* s1, prime31_t* s2)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16*2];
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B -1 ;
    
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime31_t> *aux_hash0, *aux_hash1, *aux_hash2;
    
    aux_hash0 = (s0==NULL) ? new Hash_CW4<uint32_t, prime31_t>(POW16) : 
                                new Hash_CW4<uint32_t, prime31_t>(POW16, s0);
    aux_hash1 = (s1==NULL) ? new Hash_CW4<uint32_t, prime31_t>(POW16) : 
                                new Hash_CW4<uint32_t, prime31_t>(POW16, s1);
    aux_hash2 = (s2==NULL) ? new Hash_CW4<uint32_t, prime31_t>(POW16) : 
                                new Hash_CW4<uint32_t, prime31_t>(POW16, s2);
    
    for (uint32_t i=0; i < POW16; i++){
        T0[i] = aux_hash0->element(i);
        T1[i] = aux_hash1->element(i);
    }
    for (uint32_t i=0; i < 2*POW16; i++){
        T2[i] = aux_hash2->element(i);
    }
    
    delete aux_hash0;
    delete aux_hash1;
    delete aux_hash2;
}

Hash_Tab<uint32_t>::Hash_Tab()
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16*2];
}

unsigned Hash_Tab<uint32_t>::element(uint32_t j)
{
    uint16_t high16 = j >> 16;
    uint16_t low16 = j & (mask16);
    
    return (T0[high16] ^ T1[low16] ^ T2[high16+low16]) & mask;
}

Hash<uint32_t>* Hash_Tab<uint32_t>::copy(){
    Hash_Tab<uint32_t> *result = new Hash_Tab<uint32_t>();
    result->mask = mask;
    
    for (unsigned i=0; i < POW16; i++){
        result->T0[i] = T0[i];
        result->T1[i] = T1[i];
    }
    for (unsigned i=0; i < 2*POW16; i++){
        result->T2[i] = T2[i];
    }
    return (Hash<uint32_t>*) result;
}

Hash_Tab<uint32_t>::~Hash_Tab()
{
    delete[] T0;
    delete[] T1;
    delete[] T2;
}

// Implementation for 64b
uint64_t** Hash_Tab<uint64_t>::getCauchy(){
    static uint64_t** _cauchy = GenerateCauchy4(POW16);
    return _cauchy;
}

Hash_Tab<uint64_t>::Hash_Tab(unsigned B, prime31_t* seeds0, prime31_t* seeds1, 
                                prime31_t* seeds2, prime31_t* seeds3, 
                                prime31_t* seeds4, prime31_t* seeds5, 
                                prime31_t* seeds6)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16];
    T3 = new uint16_t[POW16];
    T4 = new uint16_t[POW19];
    T5 = new uint16_t[POW19];
    T6 = new uint16_t[POW19];
    
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B-1;
    
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime31_t> aux_hash0 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds0);
    Hash_CW4<uint32_t, prime31_t> aux_hash1 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds1);
    Hash_CW4<uint32_t, prime31_t> aux_hash2 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds2);
    Hash_CW4<uint32_t, prime31_t> aux_hash3 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds3);
    for (unsigned i=0; i < POW16; i++){
        T0[i] = aux_hash0.element(i);
        T1[i] = aux_hash1.element(i);
        T2[i] = aux_hash2.element(i);
        T3[i] = aux_hash3.element(i);
    }
    Hash_CW4<uint32_t, prime31_t> aux_hash4 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds4);
    Hash_CW4<uint32_t, prime31_t> aux_hash5 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds5);
    Hash_CW4<uint32_t, prime31_t> aux_hash6 = Hash_CW4<uint32_t, prime31_t>(POW16, seeds6);
    for (unsigned i=0; i < POW19; i++){
        T4[i] = aux_hash4.element(i);
        T5[i] = aux_hash5.element(i);
        T6[i] = aux_hash6.element(i);
    }

    // Fill the rows of the Cauchy matrix
    cauchy = getCauchy();
    
}

Hash_Tab<uint64_t>::Hash_Tab(unsigned B)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16];
    T3 = new uint16_t[POW16];
    T4 = new uint16_t[POW19];
    T5 = new uint16_t[POW19];
    T6 = new uint16_t[POW19];
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B-1;
    
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime61_t> aux_hash0 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash1 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash2 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash3 = Hash_CW4<uint32_t, prime61_t>(POW16);
    for (unsigned i=0; i < POW16; i++){
        T0[i] = aux_hash0.element(i);
        T1[i] = aux_hash1.element(i);
        T2[i] = aux_hash2.element(i);
        T3[i] = aux_hash3.element(i);
    }
    Hash_CW4<uint32_t, prime61_t> aux_hash4 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash5 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash6 = Hash_CW4<uint32_t, prime61_t>(POW16);
    for (unsigned i=0; i < POW19; i++){
        T4[i] = aux_hash4.element(i);
        T5[i] = aux_hash5.element(i);
        T6[i] = aux_hash6.element(i);
    }
    
    // Fill the rows of the Cauchy matrix
    cauchy = getCauchy();
}

Hash_Tab<uint64_t>::Hash_Tab()
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16];
    T3 = new uint16_t[POW16];
    T4 = new uint16_t[POW19];
    T5 = new uint16_t[POW19];
    T6 = new uint16_t[POW19];
    
    cauchy = getCauchy();
}

unsigned Hash_Tab<uint64_t>::element(uint64_t j)
{
    uint16_t x0 = j & mask16;
    uint16_t x1 = (j >> 16) & mask16;
    uint16_t x2 = (j >> 32) & mask16;
    uint16_t x3 = (j >> 48);
    uint64_t c = cauchy[0][x0] + cauchy[1][x1] + cauchy[2][x2] + cauchy[3][x3];
    
    return (T0[x0] ^ T1[x1] ^ T2[x2] ^ T3[x3] ^ T4[c&mask21] ^ T5[(c>>21)&mask21] ^ T6[(c>>42)]) & mask;
}


Hash<uint64_t>* Hash_Tab<uint64_t>::copy(){
    Hash_Tab<uint64_t> *result = new Hash_Tab<uint64_t>();
    result->mask = mask;
    result->cauchy = cauchy;
    
    for (unsigned i=0; i < POW16; i++){
        result->T0[i] = T0[i];
        result->T1[i] = T1[i];
        result->T2[i] = T2[i];
        result->T3[i] = T3[i];
    }
    for (unsigned i=0; i < POW19; i++){
        result->T4[i] = T4[i];
        result->T5[i] = T5[i];
        result->T6[i] = T6[i];
    }
    return (Hash<uint64_t>*) result;
}

Hash_Tab<uint64_t>::~Hash_Tab()
{
    delete[] T0;
    delete[] T1;
    delete[] T2;
    delete[] T3;
    delete[] T4;
    delete[] T5;
    delete[] T6;
}

// Implementation for 128b
Hash_Tab<uint128_t>::Hash_Tab(unsigned B, prime31_t** seeds)
{
    unsigned q = 8;
    if (!isPowerOfTwo(B)) 
        throw std::domain_error("The number of buckets should be a power of 2");
    mask = B - 1;
    
    if (seeds==NULL)
    {
        seeds = new prime31_t*[2*q-1];
        for (unsigned i=0; i<2*q-1; i++)
        {
            seeds[i] = new prime31_t[4];
            for (unsigned j=0; j<4; j++)
                seeds[i][j] = random<prime31_t>();
        }
    }
    // Initialize 4-universal hash tables
    for (unsigned i = 0; i<q; i++){
        T[i] = new uint16_t[POW16];
        Hash_CW4<uint16_t, prime31_t> hash = Hash_CW4<uint16_t, prime31_t>(POW16, seeds[q]);
        for (unsigned j = 0; j<POW16; j++)
            T[i][j] = hash.element(j);
    }
    
    for (unsigned i = q; i<2*q-1; i++){
        T[i] = new uint16_t[POW20];
        Hash_CW4<uint32_t, prime31_t> hash = Hash_CW4<uint32_t, prime31_t>(POW16, seeds[q]);
        for (unsigned j = 0; j<POW20; j++)
            T[i][j] = hash.element(j);
    }
    
    // Create Cauchy look-up table
    cauchy = getCauchy();
}

CauchyRow* Hash_Tab<uint128_t>::getCauchy()
{
    static CauchyRow* _cauchy = GenerateCauchy8(POW16);
    return _cauchy;
}

unsigned Hash_Tab<uint128_t>::element(uint128_t j)
{
    uint128_t temp = mask16;
    uint16_t x0 = j & temp;
    uint16_t x1 = (j >> 16) & temp;
    uint16_t x2 = (j >> 32) & temp;
    uint16_t x3 = (j >> 48) & temp;
    uint16_t x4 = (j >> 64) & temp;
    uint16_t x5 = (j >> 80) & temp;
    uint16_t x6 = (j >> 96) & temp;
    uint16_t x7 = (j >> 112) & temp;
    uint64_t c0 = cauchy[0].v0[x0] + cauchy[1].v0[x1] + cauchy[2].v0[x2] + 
                    cauchy[3].v0[x3] + cauchy[4].v0[x4] + cauchy[5].v0[x5] + 
                    cauchy[6].v0[x6] + cauchy[7].v0[x7];
    uint64_t c1 = cauchy[0].v1[x0] + cauchy[1].v1[x1] + cauchy[2].v1[x2] + 
                    cauchy[3].v1[x3] + cauchy[4].v1[x4] + cauchy[5].v1[x5] + 
                    cauchy[6].v1[x6] + cauchy[7].v1[x7];
    uint64_t c2 = cauchy[0].v2[x0] + cauchy[1].v2[x1] + cauchy[2].v2[x2] + 
                    cauchy[3].v2[x3] + cauchy[4].v2[x4] + cauchy[5].v2[x5] + 
                    cauchy[6].v2[x6] + cauchy[7].v2[x7];
    
    // Hash every element, xor them and & mask
    return (T[0][x0] ^ T[1][x1] ^ T[2][x2] ^ T[3][x3] ^ T[4][x4] ^ T[5][x5] ^ T[6][x6] ^ T[7][x7] ^
            T[8][c0&mask21] ^ T[9][(c0>>21)&mask21] ^ T[10][(c0>>42)] ^
            T[11][c1&mask21] ^ T[12][(c1>>21)&mask21] ^
            T[13][c2&mask21] ^ T[14][(c2>>21)&mask21] ) & mask;
}

Hash<uint128_t>* Hash_Tab<uint128_t>::copy(){
    Hash_Tab<uint128_t> *result = new Hash_Tab<uint128_t>();
    result->mask = mask;
    result->cauchy = cauchy;
    
    unsigned q = 8;
    result->T = new uint16_t*[2*q-1];
    for (unsigned i = 0; i<q; i++){
        result->T[i] = new uint16_t[POW16];
        for (unsigned j = 0; j<POW16; j++)
            result->T[i][j] = T[i][j];
    }
    
    for (unsigned i = q; i<2*q-1; i++){
        result->T[i] = new uint16_t[POW20];
        for (unsigned j = 0; j<POW20; j++)
            result->T[i][j] = T[i][j];
    }
    return (Hash<uint128_t>*) result;
}

Hash_Tab<uint128_t>::~Hash_Tab()
{
    unsigned q = 8;
    for (unsigned i = 0; i<2*q-1; i++){
        delete[] T;
    }
    delete[] T;
}
