#include "simulator.h"
#include "mem_instruction.h"
inline void initialize() {
	//initializing
	halt = 0;
	cycle = 0;
	memset(lastReg, 0, 128);
	PC = 0;
	HI = 0;
	LO = 0;
	lastPC = 0;
	lastHI = 0;
	lastLO = 0;
	toggledHILO = 0; toggledMULT = 0;
	//Read iimage and dimage
	OpenFile();
	lastReg[29] = reg[29];			//stupid $sp...
}
int main(int argc , char **argv)
{
	initialize();
	iDisk_size = 1024; dDisk_size = 1024;
	if (argc == 1)
	{
		iMem_size = 64;
		dMem_size = 32;
		iPage_size = 8;
		dPage_size = 16;
		iCache_size = 16;
		iBlock_size = 4;
		iCache_associate_num = 4;
		dCache_size = 16;
		dBlock_size = 4;
		dCache_associate_num = 1;
	}
	else if (argc == 11) {
		iMem_size = atoi(argv[1]);
		dMem_size = atoi(argv[2]);
		iPage_size = atoi(argv[3]);
		dPage_size = atoi(argv[4]);
		iCache_size = atoi(argv[5]);
		iBlock_size = atoi(argv[6]);
		iCache_associate_num = atoi(argv[7]);
		dCache_size = atoi(argv[8]);
		dBlock_size = atoi(argv[9]);
		dCache_associate_num = atoi(argv[10]);
	}
	else
	{
		printf("Something wrong with argv\n");
		exit(0);
	}
	//initialize Cache things
	iPTE_init();
	iTLB_init();
	iCache_init();
	iMemory_init();
	dPTE_init();
	dTLB_init();
	dCache_init();
	dMemory_init();

  //Start simulation
  while(halt!=1)
  {
    writeRegZero=0;
    numOverflow=0;
    HILOOverWrite=0;
    memOverflow=0;
    dataMisaligned=0;
    snapShot();
	ICMP(PC);
    MachineCodeToCommands();
	printf("\n");
  }

  reporting();
  fclose(snapshot);
  //system("PAUSE");
  return 0;
}
