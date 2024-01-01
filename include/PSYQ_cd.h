#pragma once

#include <common.h>
#include <custom_types.h>
#include <PSYQ/LIBCD.h>

/*
typedef struct CdlFILTER{
    unsigned char file;         //File ID, Always 1
    unsigned char chan;         //Channel ID, Picks a song from the current music file (goes up to 7)
    unsigned short pad;         //System reserved
} CdlFILTER;*/

//int CdRead(int sectors, unsigned int * buf, int mode);
//int CdStatus(void);

int CdControl(unsigned char com, unsigned char * param, unsigned char * result);  //Tells the CD-DRIVE to do a primitive function, and someimes allows concurrent cd functions

void *CdDataCallback(void (*func)());
CdlLOC *CdIntToPos(int i, CdlLOC * p);
int CdPosToInt(CdlLOC * p);
int CdSync_(int mode, unsigned char * result);

void read_cb(unsigned char status, unsigned char *result);