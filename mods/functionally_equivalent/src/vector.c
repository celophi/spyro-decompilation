#include <custom_types.h>
#include <vector.h>
#include <memory.h>
#include <symbols.h>

#include <PSYQ/LIBGTE.h>

/**
 * @brief Scales a vector by a ratio.
 * @details For example, the vector (0, 1511, -686) scaled by denominator 1659 and numerator 1024
 * results in (1024/1659) * 1511 = 932 and (1024/1659) * -686 = -424.
 * @note 
 *     - Address: 0x800175b8
 *     - Hook: ScaleVector.s
 *     - Test: ScaleVectorTest.c
 * @param vector Input vector to be scaled.
 * @param denominator Bottom half of the ratio.
 * @param numerator Top half of the ratio.
 * @return Scaled vector.
 */
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

/**
 * @brief Calculates the length of a 2D or 3D vector using an adaptation of Newton's iteration using a lookup table.
 * @details Please refer to:
 *      - https://github.com/python/cpython/blob/main/Modules/mathmodule.c
 *      - https://stackoverflow.com/questions/65986056/is-there-a-non-looping-unsigned-32-bit-integer-square-root-function-c
 *      - https://stackoverflow.com/questions/15390807/integer-square-root-in-python
 * @note 
 *     - Address: 0x800171fc
 *     - Hook: GetVectorLength.s
 *     - Test: GetVectorLengthTest.c
 * @param vector Input vector to calculate length.
 * @param includeZ When true, indicates that the vector is 3D and the 'Z' coordinate should be included.
 * @return Length of the vector.
*/
uint GetVectorLength(const Vec3u32 *vector, const int includeZ)
{
    int RT1;
    int RT2;
    int RT3;

    gte_ldIR1(vector->X);
    gte_ldIR2(vector->Y);

    // If calculating a 3D vector, include the Z coordinate.
    int zComponent = includeZ ? vector->Z : 0;
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

/**
 * @brief Calculates the arctan of a vector to get the estimated angle.
 * @details The returned value is a number where 64 = 90 degrees, and 256 = 360 degrees.
 * @note
 *     - Address: 0x800169ac
 *     - Hook: GetAngle.s
 *     - Test: GetAngleTest.c
 * @param x Position of the X component of a vertex.
 * @param y Position of the Y component of a vertex.
 * @return Angle based on a number aligned with 64 = 90 degrees.
*/
int GetAngle(const int x, const int y)
{
    // Get the absolute values of both X and Y.
    int absX = x < 0 ? -x : x;
    int absY = y < 0 ? -y : y;

    // If ABS(X) > ABS(Y) then swap the index and divisor.
    int numerator = absX > absY ? absY : absX;
    int divisor = absX > absY ? absX : absY;

    // Make sure divisor is not zero to avoid division by zero.
    divisor = divisor == 0 ? 1 : divisor;

    int idx = (numerator << 6) / divisor;
    uint result = (uint) _AngleArray[idx];

    // Determine an offset multiplier based on the sign value of both X and Y.
    int multiplier = (x < 0)
        ? ((y < 0) ? 2 : 1)
        : ((y < 0) ? 3 : 0);

    // Determine the result from the glimmer array needs to be inverted.
    bool invert = absX < absY;

    // Handle a specific case when only one of either X and Y values are negative.
    if ((x < 0 && y >= 0) || (x >= 0 && y < 0))
    {
        invert = absX >=  absY;
    }

    // When inverted, the multiplier also gets adjusted forward.
    if (invert) 
    {
        multiplier++;
        result *= -1;
    }

    int offset = multiplier * 64;
    return offset + result;
}

/**
 * @brief Rotates a vector by a matrix.
 * @note
 *      - Address: 0x80017048
 *      - Hook: RotateVectorByMatrix.s
 *      - Test: RotateVectorByMatrixTest.c
 * @param matrix Rotational matrix to apply to the vector.
 * @param input Vector to apply the rotation to.
 * @param output Resulting vector after rotation is applied.
*/
void RotateVectorByMatrix(const RotationMatrix* matrix, Vec3u32* input, Vec3u32* output)
{
    gte_ldR11R12(matrix->R11R12);
    gte_ldR13R21(matrix->R13R21);
    gte_ldR22R23(matrix->R22R23);
    gte_ldR31R32(matrix->R31R32);
    gte_ldR33(matrix->R33);

    gte_ldIR3(input->X);
    gte_ldIR1(-input->Y);
    gte_ldIR2(-input->Z);
    
    gte_rtir();

    int x;
    int y;
    int z;
    read_mt(y, z, x);
    
    output->X = x;
    output->Y = -y;
    output->Z = -z;
}

/// @brief Sets all dimensions of a vector to zero.
/// @param vector Input vector.
void ClearVector(Vec3u32* vector)
{
    vector->X = 0;
    vector->Y = 0;
    vector->Z = 0;
}

/// @brief Scales a vector down by a power of 2.
/// @param vector Input vector.
/// @param power Exponential value to scale down by.
void ScaleVectorDown(Vec3u32* vector, uint power)
{
    // Clamp exponential decrease.
    power &= 31;

    vector->X >>= power;
    vector->Y >>= power;
    vector->Z >>= power;
}

/// @brief Scales a vector up by a power of 2.
/// @param vector Input vector.
/// @param power Exponential value to scale down by.
void ScaleVectorUp(Vec3u32* vector, uint power)
{
    // Clamp exponential increase.
    power &= 31;

    vector->X <<= power;
    vector->Y <<= power;
    vector->Z <<= power;
}

/// @brief Scales a vector by a fraction where the denominator is an exponent of the number '2'.
/// @note UNTESTED - Still need to understand the condition in game where this is used.
/// @param vector Input vector.
/// @param numerator Top number of the fraction.
/// @param denominator Bottom number of the fraction which is an exponent of the number '2'.
void ScaleVectorByExponent(Vec3u32* vector, uint numerator, uint denominator)
{
    gte_ldlvl(vector);
    gte_ldMAC1(0);
    gte_ldMAC2(0);
    gte_ldMAC3(0);
    gte_ldIR0(numerator);
    gte_gpl0();

    int x;
    int y;
    int z;

    read_mt(x, y, z);

    // Clamp denominator to integer size.
    denominator &= 31;

    // Apply the denominator while keeping the sign of the value.
    vector->X = (x < 0) ? -(-x >> denominator) : x >> denominator;
    vector->Y = (y < 0) ? -(-y >> denominator) : y >> denominator;
    vector->Z = (z < 0) ? -(-z >> denominator) : z >> denominator;
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
