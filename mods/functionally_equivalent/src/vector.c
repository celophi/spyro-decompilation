#include <custom_types.h>
#include <vector.h>
#include <memory.h>

/** @ingroup reveresed_functions
 *  @{
 */

/// @brief Adds two vectors together and puts the result in the destination.
/// @param destination Resulting vector.
/// @param first First vector as part of the sum.
/// @param second Second vector as part of the sum.
void AddVector3D(Vector3D* destination, Vector3D* first, Vector3D* second) 
{
    destination->X = first->X + second->X;
    destination->Y = first->Y + second->Y;
    destination->Z = first->Z + second->Z;
}

/// @brief Copies the source vector to the destination vector.
/// @param destination Destination vector to get copied to.
/// @param source Source vector to copy from.
void CopyVector3D(Vector3D* destination, Vector3D* source) 
{
    memcpy(destination, source, sizeof(Vector3D));
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
