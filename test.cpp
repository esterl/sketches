// Simple tests

#include <iostream>

#include "ttmath/ttmath.h"
#include "xis.h"
#include "hash.h"
#include "sketches.h"

typedef ttmath::UInt<2> bigint;
typedef ttmath::UInt<132> bigint2;

int main()
{
/**********************Some ttmath testing ************************************/
    bigint2 a = 15637580004L;
    bigint2 b = 16;
    std::cout << a % b << std::endl;

/**********************Testing Xis.h implementation****************************/

    Xi_BCH3<bigint> xi1 = Xi_BCH3<bigint>(1, 7469);
    int result = xi1.element(2500);
    
    std::cout << "Result of applying BCH 3-wise independent with s_0=1, " <<
                    "S_0=7469 over 2500(should be -1): " << result << std::endl;
    
    Xi_BCH5<bigint> xi2 = Xi_BCH5<bigint>(1, 7469, 346);
    result = xi2.element(2500);
    
    std::cout << "Result of applying BCH 5-wise independent with s_0=1, " <<
                    "S_0=7469, S_1=346 over 2500(should be 1): " << result << 
                    std::endl;
    
    Xi_EH3<bigint> xi3 = Xi_EH3<bigint>(1, 7469);
    result = xi3.element(2500);
    
    std::cout << "Result of applying EH 3-wise independent with s_0=1, " <<
                    "S_0=7469 over 2500: " << result << std::endl;
    
    Xi_CW2<bigint, bigint2> xi4 = Xi_CW2<bigint, bigint2>(4, 36);
    result = xi4.element(2500);
    
    std::cout << "Result of applying CW 2-wise independent with I1=4, " <<
                    "I2=36 over 2500(should be -1): " << result << std::endl;
    
    Xi_CW4<bigint, bigint2> xi5 = Xi_CW4<bigint, bigint2>(4, 36, 2, 1);
    result = xi4.element(2500);
    
    std::cout << "Result of applying CW 4-wise independent with I1=4, " <<
                    "I2=36, I3=2, I4=1 over 2500(should be -1): " << result << 
                    std::endl;

/**********************Testing Hash.h implementation***************************/

    Hash_CW2<bigint, bigint2> hash1 = Hash_CW2<bigint, bigint2>(4, 36, 16);
    unsigned int result2 = hash1.element(2500);
    std::cout << "Result of applying CW 2-wise independent with I1=4, " <<
                    "I2=36 and 16 buckets over 2500(should be 4): " << 
                    result2 << std::endl;
    
    Hash_CW4<bigint, bigint2> hash2 = Hash_CW4<bigint, bigint2>(4, 36, 2, 1, 16);
    result2 = hash2.element(2500);
    std::cout << "Result of applying CW 4-wise independent with I1=4, " <<
                    "I2=36, I3=2, I4=1 and 16 buckets over 2500 " << 
                    "(should be 4): " << result2 << std::endl;

/**********************Testing Sketches.h implementation***********************/

    //TODO should make sure each Sketch goes with they Xi/Hash selected:
    //AGMS sketch:
    std::cout << "AGMS Sketch " << std::endl;
    unsigned int num_buckets = 16;
    unsigned int num_rows = 16;
    Xi_BCH5<unsigned int>** xis;
    xis = new Xi_BCH5<unsigned int>*[num_buckets*num_rows];
    for (int i =0; i < num_buckets*num_rows; i++)
        xis[i] = new Xi_BCH5<unsigned int>();
    AGMS_Sketch<unsigned int> *sketch1, *sketch2;
    sketch1 = new AGMS_Sketch<unsigned int>(num_buckets, num_rows, (Xi<unsigned int>**)xis);
    sketch2 = new AGMS_Sketch<unsigned int>(num_buckets, num_rows, (Xi<unsigned int>**)xis);
    sketch1->update(5,1);
    std::cout << "Second moment of the sketch " << sketch1->second_moment() << 
                    std::endl;
    sketch1->update(5,1);
    std::cout << "Second moment of the sketch " << sketch1->second_moment() << 
                    std::endl;
    sketch2->update(5,2);
    std::cout << "Difference of the sketches " << sketch1->difference(sketch2) << 
                    std::endl;
    std::cout << "Inner join of the sketch " << sketch1->inner_join(sketch2) << 
                    std::endl;
    sketch1->clear();
    delete sketch1;
    delete sketch2;
    delete [] xis;
    
    //FAGMS Sketch
    std::cout << "FAGMS Sketch " << std::endl;
    xis = new Xi_BCH5<unsigned int>*[num_rows];
    Hash_CW2<unsigned int, unsigned long>** hashes;
    hashes = new Hash_CW2<unsigned int, unsigned long>*[num_rows];
    
    for (int i =0; i < num_rows; i++) {
        xis[i] = new Xi_BCH5<unsigned int>();
        hashes[i] = new Hash_CW2<unsigned int, unsigned long>(num_buckets);
    }
    hashes[1] = (Hash_CW2<unsigned int, unsigned long>*) hashes[2]->copy();
    FAGMS_Sketch<unsigned int> *sketch3, *sketch4;
    sketch3 = new FAGMS_Sketch<unsigned int>(num_buckets, num_rows, 
                        (Hash<unsigned int>**) hashes, (Xi<unsigned int>**)xis);
    sketch4 = new FAGMS_Sketch<unsigned int>(num_buckets, num_rows, 
                        (Hash<unsigned int>**) hashes, (Xi<unsigned int>**)xis);
    sketch3->update(5,1);
    std::cout << "Second moment of the sketch " << sketch3->second_moment() << 
                    std::endl;
    sketch3->update(5,1);
    std::cout << "Second moment of the sketch " << sketch3->second_moment() << 
                    std::endl;
    sketch4->update(5,2);
    std::cout << "Difference of the sketches " << sketch3->difference(sketch4) << 
                    std::endl;
    std::cout << "Inner join of the sketch " << sketch3->inner_join(sketch4) << 
                    std::endl;
    sketch3->clear();
    delete sketch3;
    delete sketch4;
    delete [] xis;
    delete [] hashes;
    
    //FastCount Sketch
    std::cout << "FastCount Sketch " << std::endl;
    Hash_CW4<unsigned int, unsigned long>** hash4way;
    hash4way = new Hash_CW4<unsigned int, unsigned long>*[num_rows];
    for (int i =0; i < num_rows; i++) {
        hash4way[i] = new Hash_CW4<unsigned int, unsigned long>(num_buckets);
    }
    FastCount_Sketch<unsigned int> *sketch5, *sketch6;
    sketch5 = new FastCount_Sketch<unsigned int>(num_buckets, num_rows, 
                        (Hash<unsigned int>**) hash4way);
    sketch6 = new FastCount_Sketch<unsigned int>(num_buckets, num_rows, 
                        (Hash<unsigned int>**) hash4way);
    sketch5->update(5,1);
    std::cout << "Second moment of the sketch " << sketch5->second_moment() << 
                    std::endl;
    sketch5->update(5,1);
    std::cout << "Second moment of the sketch " << sketch5->second_moment() << 
                    std::endl;
    sketch6->update(5,2);
    std::cout << "Difference of the sketches " << sketch5->difference(sketch6) << 
                    std::endl;
    std::cout << "Inner join of the sketch " << sketch5->inner_join(sketch6) << 
                    std::endl;
    sketch5->clear();
    delete sketch5;
    delete sketch6;
    delete [] hash4way;
    
    //CountMin Sketch
    std::cout << "CountMin Sketch " << std::endl;
    hashes = new Hash_CW2<unsigned int, unsigned long>*[num_rows];
    for (int i =0; i < num_rows; i++) {
        hashes[i] = new Hash_CW2<unsigned int, unsigned long>(num_buckets);
    }
    CountMin_Sketch<unsigned int> *sketch7, *sketch8;
    sketch7 = new CountMin_Sketch<unsigned int>(num_buckets, num_rows, 
                        (Hash<unsigned int>**) hashes);
    sketch8 = new CountMin_Sketch<unsigned int>(num_buckets, num_rows, 
                        (Hash<unsigned int>**) hashes);
    sketch7->update(5,1);
    std::cout << "Second moment of the sketch " << sketch7->second_moment() << 
                    std::endl;
    sketch7->update(5,1);
    std::cout << "Second moment of the sketch " << sketch7->second_moment() << 
                    std::endl;
    sketch8->update(5,2);
    std::cout << "Difference of the sketches " << sketch7->difference(sketch8) << 
                    std::endl;
    std::cout << "Inner join of the sketch " << sketch7->inner_join(sketch8) << 
                    std::endl;
    sketch7->clear();
    std::cout << "Second moment of the sketch " << sketch7->second_moment() << 
                    std::endl;
    delete sketch8;
    delete sketch7;
    delete [] hashes;
}