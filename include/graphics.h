#pragma once

#include <PSYQ/LIBGPU.h>

typedef struct {
    DRAWENV DrawEnv;
    DISPENV DispEnv;
    int* CrazyPrimList;
    int U0;
    int* PrimitiveList;
    int U1;
    int U2;
} GraphicsEnvironment;

extern GraphicsEnvironment _GfxEnvA;
extern GraphicsEnvironment _GfxEnvB;

/// @brief Initializes the display and graphics environments.
/// @note
///     - Address: 0x800122a8
///     - Hook: InitializeDisplay.s
///     - Test: InitializeDisplayTest.c
///     - Test Status: Broken (Problem with copying reserved memory?)
void InitializeDisplay();