#pragma once

typedef unsigned short u_short;

/// @brief Time-code based CD-ROM disc position.
typedef struct 
{
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;

typedef struct {
    u_char file;
    u_char chan;
    u_short pad;
} CdlFILTER;

typedef struct {
    short left;
    short right;
} SpuVolume;

typedef struct {
    SpuVolume volume;
    long reverb;
    long mix;
} SpuExtAttr;

typedef struct {
    unsigned long mask;
    SpuVolume mvol;
    SpuVolume mvolmode;
    SpuVolume mvolx;
    SpuExtAttr cd;
    SpuExtAttr ext;
} SpuCommonAttr;

enum CdPrimitive 
{
    CdlNop = 0x01,
    CdlSetloc = 0x02,
    CdlPlay = 0x03,
    CdlForward = 0x04,
    CdlBackward = 0x05,
    CdlReadN = 0x06,
    CdlStandby = 0x07,
    CdlStop = 0x08,
    CdlPause = 0x09,
    CdlMute = 0x0b,
    CdlDemute = 0x0c,
    CdlSetfilter = 0x0d,
    CdlSetmode = 0x0e,
    CdlGetparam	= 0x0f,
    CdlGetlocL = 0x10,
    CdlGetlocP = 0x11,
    CdlGetTN = 0x13,
    CdlGetTD = 0x14,
    CdlSeekL = 0x15,
    CdlSeekP = 0x16,
    CdlReadS = 0x1B,    
};

/// @brief CD-ROM Mode (used int CdlSetmode)
enum CdMode
{
    // 0: normal speed	1: double speed
    CdlModeSpeed = 0x80,
};

enum CdInterrupt 
{
    // No interrupt
    CdlNoIntr = 0x00,

    // Data ready
    CdlDataReady = 0x01,

    // Command complete
    CdlComplete = 0x02,

    // Acknowledge (reserved)
    CdlAcknowledge = 0x03,

    // End of data detected
    CdlDataEnd = 0x04,

    // Error detected
    CdlDiskError = 0x05,
};

extern int CdInit();
extern CdlCB CdReadCallback(CdlCB func);
extern int CdSync(int mode, u_char *result);
extern int CdControlB(u_char com, u_char *param, u_char *result);
extern int CdRead(int sectors, u_long *buf, int mode);
