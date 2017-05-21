#ifndef simulator_h
#define simulator_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#pragma warning(disable:4996)
#define MEM_MAX_SIZE 1024

char *iimageBuf,*dimageBuf;
FILE *iimage, *dimage, *error_dump, *snapshot, *report;

int writeRegZero,numOverflow,HILOOverWrite,halt;
int memOverflow,dataMisaligned;

int toggledHILO, toggledMULT;

unsigned char iMemory[1024];
unsigned iPos,address;
unsigned op, rs, rt, rd, func, shamt, immediate;

unsigned char dMemory[1024];
unsigned dAddr, dPos;

unsigned reg[32], PC, HI, LO;
unsigned lastReg[32], lastPC, lastHI, lastLO;

unsigned cycle;

int iDisk_size;
int iMem_size;
int iPage_size;
int iCache_size;
int iBlock_size;
int iCache_associate_num;
int iCache_entries;
int iPTE_entries;
int iTLB_entries;
int iPage_offset;
int iMem_entries;
int iTLB_hit;
int iTLB_miss;
int iPTE_hit;
int iPTE_miss;
int iCACHE_hit;
int iCACHE_miss;

int dTLB_hit;
int dTLB_miss;
int dPTE_hit;
int dPTE_miss;
int dCACHE_hit;
int dCACHE_miss;

int IVPN;
int IPPN;

int DVPN;
int DPPN;


int dDisk_size;
int dMem_size;
int dPage_size;
int dCache_size;
int dBlock_size;
int dCache_associate_num;
int dCache_entries;
int dPTE_entries;
int dTLB_entries;
int dPage_offset;
int dMem_entries;

typedef struct _TLB {
	int VPN;
	int PPN;
	int tag;
	int valid;
	int last_cycle;
} TLB;

typedef struct _PTE {
	int PPN;
	int valid;
} PTE;

typedef struct _CACHE {
	int tag;
	int MRU;
	int valid;
} CACHE;

typedef struct _MEMORY {
	int last_cycle;
	int valid;
} MEMORY;

TLB *iTLB, *dTLB;
PTE *iPTE, *dPTE;
CACHE **iCACHE, **dCACHE;
MEMORY *iMEMORY, *dMEMORY;

void ICMP(int);
void DCMP(int);
int findITLB(int);

int findIPTE(int);

void updateIPTE(int);

void updateITLB(int);

int findICACHE(int);

void updateICACHE(int);

int findDTLB(int);

int findDPTE(int);

void updateDPTE(int);

void updateDTLB(int);

int findDCACHE(int);

void updateDCACHE(int);
void OpenFile();
void iimageParser();
void dimageParser();
void snapShot();
void printParsed();
void reporting();
void iTLB_init();
void dTLB_init();
void iPTE_init();
void dPTE_init();
void iCache_init();
void dCache_init();
void iMemory_init();
void dMemory_init();

#endif
