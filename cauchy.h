#ifndef SKETCHES_CAUCHY_H
#define SKETCHES_CAUCHY_H

//Creates the multiplication table for n of length rows in the prime field
inline uint32_t* multiplicationTable(uint32_t n, uint32_t rows, uint32_t prime){
    uint32_t *result = new uint32_t[rows];
    
    for (uint32_t i=0; i<rows; i++){
        result[i] = (n*i) % prime;
    }
    return result;
}

//Creates the multiplication tables for a Cauchy matrix of size q*(q-1)
inline uint32_t** cauchyTables(uint32_t rows, unsigned q){
    uint32_t prime = (1 << 16) + 1;
    // Array with the inverse of 1, 2, 3, etc. mod prime
    uint16_t invmodp[14] = { 1, 32769, 21846, 49153, 26215, 10923, 18725, 57345, 
                            7282, 45876, 5958, 38230, 15124, 42131};
    
    uint32_t **tables = new uint32_t*[2*(q-1)];
    for (unsigned i=0; i < 2*(q-1); i++){
        tables[i] = multiplicationTable(invmodp[i], rows, prime);
    }
    
    return tables;
}

// Fit 3 the components into a single uint64_t
inline uint64_t* compressRow(uint32_t rows, uint32_t* c0, uint32_t* c1, uint32_t* c2){
    uint64_t *result = new uint64_t[rows];
    
    for (uint32_t i=0; i<rows; i++){
        uint64_t x0, x1, x2;
        x0 = c0[i]; x1 = c1[i]; x2 = c2[i];
        result[i] = x0 + (x1<<21) + (x2<<42);
    }
    return result;
}

// Fit 2 the components into a single uint64_t
inline uint64_t* compressRow(uint32_t rows, uint32_t* c0, uint32_t* c1){
    uint64_t *result = new uint64_t[rows];
    
    for (uint32_t i=0; i<rows; i++){
        uint64_t x0, x1;
        x0 = c0[i]; x1 = c1[i];
        result[i] = x0 + (x1<<21);
    }
    return result;
}

// Generates a compressed Cauchy matrix of 4 rows and 3 compressed columns
inline uint64_t** GenerateCauchy4(uint32_t rows){
    uint32_t** tables = cauchyTables(rows, 4);
    unsigned q = 4;
    
    uint64_t** result = new uint64_t*[4];
    for (unsigned i=0; i<q; i++)
    {
        result[i] = compressRow(rows, tables[i], tables[i+1], tables[i+2]);
    }
    
    for (unsigned i=0; i< 2*(q-1); i++){
        delete [] tables[i];
    }
    delete [] tables;
    return result;
}

struct CauchyRow{
    uint64_t *v0, *v1, *v2;
};

// Generates a compressed Cauchy matrix of 8 rows and 7 compressed columns
inline CauchyRow* GenerateCauchy8(uint32_t rows){
    unsigned q = 8;
    uint32_t** tables = cauchyTables(rows, 8);
    
    CauchyRow* matrix = new CauchyRow[8];
    
    for (unsigned i=0; i<q; i++)
        matrix[i].v0 = compressRow(rows, tables[i], tables[i+1], tables[i+2]);
    
    for (unsigned i=0; i<q; i++)
    {
        unsigned j = i+3;
        if (j<q)
            matrix[i].v1 = matrix[j].v0;
        else
            matrix[i].v1 = compressRow(rows, tables[j], tables[j+1]);
    }
    
    for (unsigned i=0; i<q-2; i++)
    {
        matrix[i].v2 = matrix[i+2].v1;
    }
    matrix[q-2].v2 = compressRow(rows, tables[q-2+5], tables[q-2+6]);
    matrix[q-1].v2 = compressRow(rows, tables[q-1+5], tables[q-1+6]);

    for (unsigned i=0; i< 2*(q-1); i++){
        delete [] tables[i];
    }
    delete [] tables;
    
    return matrix;
}
#endif // SKETCHES_CAUCHY_H
