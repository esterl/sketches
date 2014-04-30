// Simple tests

#include <iostream>

#include "ttmath/ttmath.h"
#include "xis.h"
#include "hash.h"

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

}
