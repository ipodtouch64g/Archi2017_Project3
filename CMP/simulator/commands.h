#ifndef commands_h
#define commands_h
#include"simulator.h"
void R_Command(char*);
void I_Command(char*);
void J_Command(char*);
void detectWriteRegZero();
void detectNumOverflow(int , int , int );
void detectHILOWrite(char*);
int detectMemOverflow(int); 
int detectDataMisaligned(int);
void findOP();
void findRSRTRD();
void findFUNC();
void findSHAMT();
void findSignedIMMEDIATE();
void findUnsignedIMMEDIATE();
void findADDR();
#endif