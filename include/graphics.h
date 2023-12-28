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