#include "mersenne.h"
#include "random.h"

/**************************Hash_CW2 implementation*****************************/

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(T2 I1, T2 I2, unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(I1, mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(I2, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::Hash_CW2(unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW2<T1,T2>::~Hash_CW2()
{
}

template<typename T1, typename T2>
unsigned int Hash_CW2<T1,T2>::element(T1 j){
    T2 result = j;
    result = mersenne_modulus(result*seeds[1], mersenne_exponent) + seeds[0];
    result = mersenne_modulus(result, mersenne_exponent);
    T2 temp = num_buckets;
    unsigned int res = result % temp;
    return res;
}

template<typename T1, typename T2>
Hash<T1>* Hash_CW2<T1,T2>::copy(){
    Hash_CW2<T1,T2> *result = new Hash_CW2<T1,T2>(seeds[0], seeds[1], 
                                                    num_buckets);
    return (Hash<T1>*) result;
}


/**************************Hash_CW4 implementation*****************************/

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(T2 I1, T2 I2, T2 I3, T2 I4, unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(I1, mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(I2, mersenne_exponent);
    seeds[2] = mersenne_modulus<T1>(I3, mersenne_exponent);
    seeds[3] = mersenne_modulus<T1>(I4, mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::Hash_CW4(unsigned int B)
{
    mersenne_exponent = get_mersenne_exponent<T1>();
    num_buckets = B;
    seeds[0] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[1] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[2] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
    seeds[3] = mersenne_modulus<T1>(random<T2>(), mersenne_exponent);
}

template<typename T1, typename T2>
Hash_CW4<T1,T2>::~Hash_CW4()
{
}

template<typename T1, typename T2>
unsigned int Hash_CW4<T1,T2>::element(T1 j)
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
    T2 temp = num_buckets;
    unsigned int res = result % temp;
    return res;
}

template<typename T1, typename T2>
Hash<T1>* Hash_CW4<T1,T2>::copy(){
    Hash_CW4<T1,T2> * result = new Hash_CW4<T1,T2>(seeds[0], seeds[1], 
                                                    seeds[2], seeds[3], 
                                                    num_buckets);
    return (Hash<T1>*) result;
}

/**************************Hash_Tab implementation*****************************/
const uint32_t POW16 = 65536;   // 2^16
const uint32_t POW21 = 2097152; // 2^21
const uint64_t mask16 = 0xFFFF;
const uint64_t mask21 = 0x1FFFFF;

// Implementation for 16b
Hash_Tab<uint16_t>::Hash_Tab(uint64_t I1, uint64_t I2, uint64_t I3, uint64_t I4, unsigned int B)
{
    table = new uint16_t[POW16];
    num_buckets = B;
    
    // Use Hash_CW4 to fill the table:
    Hash_CW4<uint16_t, uint64_t> aux_hash = Hash_CW4<uint16_t, uint64_t>(I1, I2, I3, I4, B);
    for (unsigned int i=0; i < POW16; i++){
        table[i] = aux_hash.element(i);
    }
}

Hash_Tab<uint16_t>::Hash_Tab(unsigned int B)
{
    table = new uint16_t[POW16];
    num_buckets = B;
    
    // Use Hash_CW4 to fill the table:
    Hash_CW4<uint16_t, uint32_t> aux_hash = Hash_CW4<uint16_t, uint32_t>(B);
    for (unsigned int i=0; i < POW16; i++){
        table[i] = aux_hash.element(i);
    }
}

// Empty constructor (for copy)
Hash_Tab<uint16_t>::Hash_Tab()
{
    table = new uint16_t[POW16];
}

unsigned int Hash_Tab<uint16_t>::element(uint16_t j){
    return table[j];
}

Hash<uint16_t>* Hash_Tab<uint16_t>::copy(){
    Hash_Tab<uint16_t> *result = new Hash_Tab<uint16_t>();
    result->num_buckets = num_buckets;
    
    for (unsigned int i=0; i < POW16; i++){
        result->table[i] = table[i];
    }
    return (Hash<uint16_t>*) result;
}

Hash_Tab<uint16_t>::~Hash_Tab()
{
    delete[] table;
}

// Implementation for 32b
Hash_Tab<uint32_t>::Hash_Tab(prime61_t* seeds0, prime61_t* seeds1, prime61_t* seeds2, unsigned int B)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16*2];
    num_buckets = B;
    
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime61_t> aux_hash0 = Hash_CW4<uint32_t, prime61_t>(seeds0[0], seeds0[1], seeds0[2], seeds0[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash1 = Hash_CW4<uint32_t, prime61_t>(seeds1[0], seeds1[1], seeds0[2], seeds1[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash2 = Hash_CW4<uint32_t, prime61_t>(seeds2[0], seeds2[1], seeds2[2], seeds2[3], POW16);
    for (unsigned int i=0; i < POW16; i++){
        T0[i] = aux_hash0.element(i);
        T1[i] = aux_hash1.element(i);
    }
    for (unsigned int i=0; i < 2*POW16; i++){
        T2[i] = aux_hash2.element(i);
    }
}

Hash_Tab<uint32_t>::Hash_Tab(unsigned int B)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16*2];
    num_buckets = B;
    
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime61_t> aux_hash0 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash1 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash2 = Hash_CW4<uint32_t, prime61_t>(POW16);
    for (unsigned int i=0; i < POW16; i++){
        T0[i] = aux_hash0.element(i);
        T1[i] = aux_hash1.element(i);
    }
    for (unsigned int i=0; i < 2*POW16; i++){
        T2[i] = aux_hash2.element(i);
    }
}

Hash_Tab<uint32_t>::Hash_Tab()
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16*2];
}

unsigned int Hash_Tab<uint32_t>::element(uint32_t j)
{
    uint16_t high16 = j >> 16;
    uint16_t low16 = j & (mask16);
    
    // For num_buckets which are a power of 2 the modulus is not necessary
    return (T0[high16] ^ T1[low16] ^ T2[high16+low16]) % num_buckets;
}

Hash<uint32_t>* Hash_Tab<uint32_t>::copy(){
    Hash_Tab<uint32_t> *result = new Hash_Tab<uint32_t>();
    result->num_buckets = num_buckets;
    
    for (unsigned int i=0; i < POW16; i++){
        result->T0[i] = T0[i];
        result->T1[i] = T1[i];
    }
    for (unsigned int i=0; i < 2*POW16; i++){
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

// TODO Check if with tables initialized with hash functions of 32b is ok
// Implementation for 64b
inline uint32_t* MultiplicationTable(uint32_t n, uint32_t rows, uint32_t prime){
    uint32_t *result = new uint32_t[rows];
    
    for (uint32_t i=0; i<rows; i++){
        result[i] = (n*i) % prime;
    }
    return result;
}

// Fit all the components into a single uint64_t
inline uint64_t* CauchyRow(uint32_t rows, uint32_t* c0, uint32_t* c1, uint32_t* c2){
    uint64_t *result = new uint64_t[rows];
    
    for (uint32_t i=0; i<rows; i++){
        uint64_t x0, x1, x2;
        x0 = c0[i]; x1 = c1[i]; x2 = c2[i];
        result[i] = x0 + (x1<<21) + (x2<<42);
    }
    return result;
}

inline uint64_t** GenerateCauchy(uint32_t rows){
    uint32_t prime = (1 << 16) + 1;
    uint16_t mod1 = 1; // 1 mod prime
    uint16_t mod1_2 = 32769; // 1/2 mod prime
    uint16_t mod1_3 = 21846; // 1/3 mod prime
    uint16_t mod1_4 = 49153; // 1/4 mod prime
    uint16_t mod1_5 = 26216; // 1/5 mod prime
    uint16_t mod1_6 = 10923; // 1/6 mod prime

    uint32_t *t1_1, *t1_2, *t1_3, *t1_4, *t1_5, *t1_6;
    t1_1 = MultiplicationTable(mod1, rows, prime);
    t1_2 = MultiplicationTable(mod1_2, rows, prime);
    t1_3 = MultiplicationTable(mod1_3, rows, prime);
    t1_4 = MultiplicationTable(mod1_4, rows, prime);
    t1_5 = MultiplicationTable(mod1_5, rows, prime);
    t1_6 = MultiplicationTable(mod1_6, rows, prime);
    
    uint64_t** result = new uint64_t*[4];
    result[0] = CauchyRow(rows, t1_1, t1_2, t1_3);
    result[1] = CauchyRow(rows, t1_2, t1_3, t1_4);
    result[2] = CauchyRow(rows, t1_3, t1_4, t1_5);
    result[3] = CauchyRow(rows, t1_4, t1_5, t1_6);
    
    delete [] t1_1;
    delete [] t1_2;
    delete [] t1_3;
    delete [] t1_4;
    delete [] t1_5;
    delete [] t1_6;
    return result;
}

Hash_Tab<uint64_t>::Hash_Tab(prime61_t* seeds0, prime61_t* seeds1, 
                                prime61_t* seeds2, prime61_t* seeds3, 
                                prime61_t* seeds4, prime61_t* seeds5, 
                                prime61_t* seeds6, unsigned int B)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16];
    T3 = new uint16_t[POW16];
    T4 = new uint16_t[POW21];
    T5 = new uint16_t[POW21];
    T6 = new uint16_t[POW21];

    num_buckets = B;
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime61_t> aux_hash0 = Hash_CW4<uint32_t, prime61_t>(seeds0[0], seeds0[1], seeds0[2], seeds0[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash1 = Hash_CW4<uint32_t, prime61_t>(seeds1[0], seeds1[1], seeds0[2], seeds1[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash2 = Hash_CW4<uint32_t, prime61_t>(seeds2[0], seeds2[1], seeds2[2], seeds2[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash3 = Hash_CW4<uint32_t, prime61_t>(seeds3[0], seeds3[1], seeds3[2], seeds3[3], POW16);
    for (unsigned int i=0; i < POW16; i++){
        T0[i] = aux_hash0.element(i);
        T1[i] = aux_hash1.element(i);
        T2[i] = aux_hash2.element(i);
        T3[i] = aux_hash3.element(i);
    }
    Hash_CW4<uint32_t, prime61_t> aux_hash4 = Hash_CW4<uint32_t, prime61_t>(seeds4[0], seeds4[1], seeds4[2], seeds4[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash5 = Hash_CW4<uint32_t, prime61_t>(seeds5[0], seeds5[1], seeds5[2], seeds5[3], POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash6 = Hash_CW4<uint32_t, prime61_t>(seeds6[0], seeds6[1], seeds6[2], seeds6[3], POW16);
    for (unsigned int i=0; i < POW21; i++){
        T4[i] = aux_hash4.element(i);
        T5[i] = aux_hash5.element(i);
        T6[i] = aux_hash6.element(i);
    }

    // Fill the rows of the Cauchy matrix
    cauchy = GenerateCauchy(POW16);
    
}

Hash_Tab<uint64_t>::Hash_Tab(unsigned int B)
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16];
    T3 = new uint16_t[POW16];
    T4 = new uint16_t[POW21];
    T5 = new uint16_t[POW21];
    T6 = new uint16_t[POW21];

    num_buckets = B;
    
    // Use Hash_CW4 to fill the tables:
    Hash_CW4<uint32_t, prime61_t> aux_hash0 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash1 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash2 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash3 = Hash_CW4<uint32_t, prime61_t>(POW16);
    for (unsigned int i=0; i < POW16; i++){
        T0[i] = aux_hash0.element(i);
        T1[i] = aux_hash1.element(i);
        T2[i] = aux_hash2.element(i);
        T3[i] = aux_hash3.element(i);
    }
    Hash_CW4<uint32_t, prime61_t> aux_hash4 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash5 = Hash_CW4<uint32_t, prime61_t>(POW16);
    Hash_CW4<uint32_t, prime61_t> aux_hash6 = Hash_CW4<uint32_t, prime61_t>(POW16);
    for (unsigned int i=0; i < POW21; i++){
        T4[i] = aux_hash4.element(i);
        T5[i] = aux_hash5.element(i);
        T6[i] = aux_hash6.element(i);
    }
    
    // Fill the rows of the Cauchy matrix
    cauchy = GenerateCauchy(POW16);
}

Hash_Tab<uint64_t>::Hash_Tab()
{
    T0 = new uint16_t[POW16];
    T1 = new uint16_t[POW16];
    T2 = new uint16_t[POW16];
    T3 = new uint16_t[POW16];
    T4 = new uint16_t[POW21];
    T5 = new uint16_t[POW21];
    T6 = new uint16_t[POW21];
    
    cauchy = GenerateCauchy(POW16);
}

unsigned int Hash_Tab<uint64_t>::element(uint64_t j)
{
    uint16_t x0 = j & mask16;
    uint16_t x1 = (j >> 16) & mask16;
    uint16_t x2 = (j >> 32) & mask16;
    uint16_t x3 = (j >> 48);
    uint64_t c = cauchy[0][x0] + cauchy[1][x1] + cauchy[2][x2] + cauchy[3][x3];
    
    return (T0[x0] ^ T1[x1] ^ T2[x2] ^ T3[x3] ^ T4[c&mask21] ^ T5[(c>>21)&mask21] ^ T6[(c>>42)]) % num_buckets;
}


Hash<uint64_t>* Hash_Tab<uint64_t>::copy(){
    Hash_Tab<uint64_t> *result = new Hash_Tab<uint64_t>();
    result->num_buckets = num_buckets;
    
    for (unsigned int i=0; i < POW16; i++){
        result->T0[i] = T0[i];
        result->T1[i] = T1[i];
        result->T2[i] = T2[i];
        result->T3[i] = T3[i];
    }
    for (unsigned int i=0; i < POW21; i++){
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
    
    delete[] cauchy[0]; delete[] cauchy[1]; delete[] cauchy[2]; delete[] cauchy[3]; 
    
    delete[] cauchy;
}


