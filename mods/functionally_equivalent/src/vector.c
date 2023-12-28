#include <custom_types.h>
#include <vector.h>
#include <memory.h>
#include <symbols.h>

#include <PSYQ/LIBGTE.h>

/// @brief Calculates the length of a 2D or 3D vector using an adaptation of Newton's iteration using a lookup table.
/// @details Please refer to: https://github.com/python/cpython/blob/main/Modules/mathmodule.c \n
/// https://stackoverflow.com/questions/65986056/is-there-a-non-looping-unsigned-32-bit-integer-square-root-function-c \n
/// https://stackoverflow.com/questions/15390807/integer-square-root-in-python
/// @param vector 3D or 2D vector.
/// @param includeZComponent When supplying a 3D vector, this should be set to 1.
/// @return Length of the vector.
uint GetVectorLength(Vec3u32 *vector, int includeZComponent)
{
    int RT1;
    int RT2;
    int RT3;

    gte_ldIR1(vector->X);
    gte_ldIR2(vector->Y);

    // If calculating a 3D vector, include the Z coordinate.
    int zComponent = includeZComponent ? vector->Z : 0;
    gte_ldIR3(zComponent);

    // Calculate the square and get the sum.     
    gte_sqr0();
    read_mt(RT1, RT2, RT3);
    int sum = RT1 + RT2 + RT3;

    uint result = 0;

    if (sum != 0) 
    {
        // Calculate the number of zeros leading in front of the sum.
        gte_ldLZCS(sum);
        result = gte_stLZCR();
        result &= 0xFFFFFFFE;
                        
        if ((int)(result - 24) < 0) 
        {
            sum = sum >> (24 - result & 0x1F);
        }
        else 
        {
            sum = sum << (result - 24 & 0x1F);
        }

        result = (int)_SquareRootLookupTable[sum + -64] << ((int)(31 - result) >> 1 & 0x1FU);
    }

    return result >> 12;
}

/// @brief Scales a vector by a ratio.
/// @param vector Input vector
/// @param denominator Bottom half of a ratio.
/// @param numerator Top half of a ratio.
void ScaleVector(Vec3u32* vector, int denominator, int numerator)
{
    int ratio = (numerator << 12) / denominator;

    gte_ldlvl(vector);
    gte_ldMAC1(0);
    gte_ldMAC2(0);
    gte_ldMAC3(0);
    gte_ldIR0(ratio);
    gte_gpl0();

    int RT1;
    int RT2;
    int RT3;
    read_mt(RT1,RT2,RT3);

    vector->X = RT1 >> 12;
    vector->Y = RT2 >> 12;
    vector->Z = RT3 >> 12;
}











/** @ingroup reveresed_functions
 *  @{
 */

/// @brief Adds two vectors together and puts the result in the destination.
/// @param destination Resulting vector.
/// @param first First vector as part of the sum.
/// @param second Second vector as part of the sum.
void AddVector3D(Vec3u32* destination, Vec3u32* first, Vec3u32* second) 
{
    destination->X = first->X + second->X;
    destination->Y = first->Y + second->Y;
    destination->Z = first->Z + second->Z;
}

/// @brief Copies the source vector to the destination vector.
/// @param destination Destination vector to get copied to.
/// @param source Source vector to copy from.
void CopyVector3D(Vec3u32* destination, Vec3u32* source) 
{
    memcpy(destination, source, sizeof(Vec3u32));
}

/**
 * @brief 
 * @details
 
 * @param uint* outputMatrix 
 * @param uint* inputMatrix 
  
 * @note
 */
void MatrixTranspose(uint* outputMatrix, uint* inputMatrix)

{
  uint A12;
  uint A23;
  uint A21;
  uint A31;
  uint A13;
  uint A23A22;
  uint A32;
  uint A33;

  uint B13;
  uint B32;
  
  A23A22 = inputMatrix[2];
  A12 = *inputMatrix & 0xffff0000;
  A21 = inputMatrix[1] & 0xffff0000;
  A13 = inputMatrix[1] ^ A21;
  B13 = A13 & 0xffff;
  A31 = inputMatrix[3] & 0xffff;
  A32 = inputMatrix[3] ^ A31;
  A23 = A23A22 & 0xffff0000;
  B32 = A32 & 0xffff0000;
  *outputMatrix = *inputMatrix ^ A12 | A21;
  outputMatrix[1] = (A13 | A12) ^ B13 | A31;
  outputMatrix[2] = A23A22 ^ A23 | B32;
  outputMatrix[3] = (A32 | B13) ^ B32 | A23;
  outputMatrix[4] = inputMatrix[4];
  return;
}

// void MatrixTranspose(unsigned int *outputMatrix, unsigned int *inputMatrix)

// {

//   *outputMatrix = (*inputMatrix & 0xffff) + (inputMatrix[1] & 0xffff0000);
//   outputMatrix[1] = (inputMatrix[3] & 0xffff) + (*inputMatrix & 0xffff0000);
//   outputMatrix[2] = (inputMatrix[2] & 0xffff) + (inputMatrix[3] & 0xffff0000);
//   outputMatrix[3] = (inputMatrix[1] & 0xffff) + (inputMatrix[2] & 0xffff0000);
//   outputMatrix[4] = inputMatrix[4];
//   return;
// }


/** @} */ // end of reveresed_functions
