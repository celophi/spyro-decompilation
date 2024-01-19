#pragma once

#include <moby.h>
#include <custom_types.h>
#include <shapes.h>

#define storeRegisters(r0)                         \
    __asm__ volatile(                              \
        "sw $s0, 0(%0);"                           \
        "sw $s1, 4(%0);"                           \
        "sw $s2, 8(%0);"                           \
        "sw $s3, 12(%0);"                          \
        "sw $s4, 16(%0);"                          \
        "sw $s5, 20(%0);"                          \
        "sw $s6, 24(%0);"                          \
        "sw $s7, 28(%0);"                          \
        "sw $gp, 32(%0);"                          \
        "sw $sp, 36(%0);"                          \
        "sw $fp, 40(%0);"                          \
        "sw $ra, 44(%0);"                          \
        :                                          \
        : "r"(r0)                                  \
        :                                          \
    )


#define doScratchCalc(bad, scratch) \
    __asm__ volatile(   \
        ".set push;" \
        ".set noreorder;" \
        "sll         %1, %0, 5;" \
        "lui         $t5, 0x0001;" \
        "sub         $a1, %0, $t5;" \
        "lui         $t6, 0x0100;" \
        "bgtz        $a1, LAB_8004ee1c;" \
        "sub         $a1, %0, $t6;" \
        "addi        %1, %1, 0x1;" \
        "LAB_8004ee1c:;" \
        "bltz        $a1, LAB_8004ee28;" \
        "sll         $a1, %0, 16;" \
        "addi        %1, %1, 0x2;" \
        "LAB_8004ee28:;" \
        "lui         $t7, 0x0200;" \
        "bgtz        $a1, LAB_8004ee34;" \
        "sub         $a1, $a1, $t7;" \
        "addi        %1, %1, 0x4;" \
        "LAB_8004ee34:;" \
        "bltz        $a1, LAB_8004ee40;" \
        "nop;" \
        "addi        %1, %1, 0x8;" \
        "LAB_8004ee40:;" \
        "nop;" \
        ".set pop;" \
        : "+r" (bad), "+r" (scratch) \
        : \
        : "$a1", "$t5", "$t6", "$t7" \
)



typedef struct
{
    int U1;
    int U2;
    int U3;
    int U4;
    int U5;
    int U6;
    uint U7;
    int U8;
} SkyboxU0;
_Static_assert(sizeof(SkyboxU0) == 32);

/**
 * @brief Adds a primitive to a linked list.
 * @details Seems to be a staging area that gets turned into an ordering table later.
 * @note
 *      - Address: 0x800168dc
 *      - Hook: AddPrimitiveToList.s
 *      - Test: AddPrimitiveToListTest.c
 * @param primitive Reference to a primitive to stage.
*/
void AddPrimitiveToList(P_TAG *primitive);

/// @brief Calculates the 8-bit difference between the supplied value and a timer, then clamps between 0 and 127.
/// @param value Supplied value.
/// @param timer In most cases, the value of a cyclical timer in the game.
/// @return Calculated value.
int GetClampedDifference(int value, int timer);



/**
 * @brief Draws a black textbox with a gold border. You can specify the bounds
 * @details Calls Draw primitive with the interesting hack they use for the alpha channel to achieve the black background, and also 4 draw lines for the border.

 * @param int xBound1 - the first x boundry of the box
 * @param int xBound2 - the second x boundry of the box
 * @param int yBound1 - the first y boundry of the box
 * @param int yBound2 - the second y boundry of the box

 * @note Function: DrawTextbox \n
 * Original Address: 0x8001860C \n
 * Hook File: draw_textbox.s \n
 * Prototype: draw_stuff.h \n
 * Amount of instructions: MORE IN MODDERN GCC (https://decomp.me/scratch/MrfVL) \n
*/
void DrawTextbox(int xBound1, int xBound2, int yBound1, int yBound2);

void DrawSkybox(int option,RotationMatrix *cameraA,RotationMatrix *cameraB);