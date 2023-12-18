#pragma once

#include <custom_types.h>

typedef struct {
    unsigned addr: 24;
    unsigned len: 8;
    byte r0;
    byte g0;
    byte b0;
    byte code;
} P_TAG;