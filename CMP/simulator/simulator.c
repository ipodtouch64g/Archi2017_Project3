#include "simulator.h"

// Open image files and output files
void OpenFile()
{

        unsigned len_iimage=0, len_dimage=0;


        //error_dump = fopen("error_dump.rpt","w");
        snapshot = fopen("snapshot.rpt","w");
        iimage = fopen("iimage.bin","rb");
        dimage = fopen("dimage.bin","rb");


        fseek(iimage,0,SEEK_END);
        fseek(dimage,0,SEEK_END);

        len_iimage = (unsigned)ftell(iimage);
        len_dimage = (unsigned)ftell(dimage);

        rewind(iimage);
        rewind(dimage);

        iimageBuf = (char*)malloc(len_iimage*sizeof(char));
        dimageBuf = (char*)malloc(len_dimage*sizeof(char));

        //Finally read it into buffer
        fread(iimageBuf,1,len_iimage,iimage);
        fread(dimageBuf,1,len_dimage,dimage);

        fclose(iimage);
        fclose(dimage);
        iimageParser();
        dimageParser();
       //printParsed();
}
//Deal with iimage
void iimageParser()
{
        unsigned tmp=0,lineNum=0;

        //First 4 bytes is initial value of PC
        for(int i=0; i<4; i++)
                tmp = (tmp<<8) + (unsigned char)iimageBuf[i];
        PC = tmp;

        //Second 4 bytes is # of how many lines of instructions below
        for(int i=4; i<8; i++)
                lineNum = (lineNum<<8) + (unsigned char)iimageBuf[i];

        //Others are instructions,write them to iMemory
        for(int i=8; i<8+lineNum*4; i++)
                iMemory[tmp++] = iimageBuf[i];
}
//Deal with dimage
void dimageParser()
{
        unsigned tmp=0,lineNum=0,index=0;

        //First 4 bytes is initial value of SP
        for(int i=0; i<4; i++)
                tmp = (tmp<<8) + (unsigned char)dimageBuf[i];
        reg[29] = tmp;

        //Second 4 bytes is # of how many lines of instructions below
        for(int i=4; i<8; i++)
                lineNum = (lineNum<<8) + (unsigned char)dimageBuf[i];

        //Others are instructions,write them to dMemory
        for(int i=8; i<8+lineNum*4; i++)
                dMemory[index++] = dimageBuf[i];
}
//Debug
void printParsed()
{
        printf("PC: 0x%08X\n", PC);
        printf("HI: 0x%08X\n", HI);
        printf("LO: 0x%08X\n", LO);
        for(int i=0; i<1024; i+=4)
                printf("iMemory at 0x%08X : 0x%02X%02X%02X%02X\n",i,iMemory[i],iMemory[i+1],iMemory[i+2],iMemory[i+3]);
        for(int i=0; i<1024; i+=4)
                printf("dMemory at 0x%08X : 0x%02X%02X%02X%02X\n",i,dMemory[i],dMemory[i+1],dMemory[i+2],dMemory[i+3]);
}
void reporting() 
{
	report = fopen("report.rpt","wb");
	fprintf(report, "ICache :\n");
	fprintf(report, "# hits: %u\n", iCACHE_hit);
	fprintf(report, "# misses: %u\n\n", iCACHE_miss);
	fprintf(report, "DCache :\n");
	fprintf(report, "# hits: %u\n", dCACHE_hit);
	fprintf(report, "# misses: %u\n\n", dCACHE_miss);
	fprintf(report, "ITLB :\n");
	fprintf(report, "# hits: %u\n", iTLB_hit);
	fprintf(report, "# misses: %u\n\n", iTLB_miss);
	fprintf(report, "DTLB :\n");
	fprintf(report, "# hits: %u\n", dTLB_hit);
	fprintf(report, "# misses: %u\n\n", dTLB_miss);
	fprintf(report, "IPageTable :\n");
	fprintf(report, "# hits: %u\n", iPTE_hit);
	fprintf(report, "# misses: %u\n\n", iPTE_miss);
	fprintf(report, "DPageTable :\n");
	fprintf(report, "# hits: %u\n", dPTE_hit);
	fprintf(report, "# misses: %u\n\n", dPTE_miss);
	fclose(report);
}
void snapShot()
{
        fprintf(snapshot, "cycle %d\n", cycle);
        //printf("cycle %d\n", cycle);
        if(cycle==0)
        {
                for(int i=0; i<32; i++)
                {
                        fprintf(snapshot, "$%02d: 0x", i);
                        fprintf(snapshot, "%08X\n", reg[i]);
                        //printf("$%02d: 0x", i);
                        //printf("%08X\n", reg[i]);
                }
                fprintf(snapshot, "$HI: 0x%08X\n",HI);
                fprintf(snapshot, "$LO: 0x%08X\n",LO);
                fprintf(snapshot, "PC: 0x%08X\n\n\n",PC);
                //printf("$HI: 0x%08X\n", HI);
                //printf("$LO: 0x%08X\n", LO);
                //printf("PC: 0x%08X\n\n\n", PC);
        }

        else
        {
                for(int i=0; i<32; i++)
                {
                        if (lastReg[i] != reg[i])
                        {
                                fprintf(snapshot, "$%02d: 0x", i);
                                fprintf(snapshot, "%08X\n", reg[i]);
                                //printf("$%02d: 0x", i);
                                //printf("%08X\n", reg[i]);
                        }
                        lastReg[i] = reg[i];
                }
                if (lastHI != HI)
                {
                        fprintf(snapshot, "$HI: 0x%08X\n", HI);
                        //printf("$HI: 0x%08X\n", HI);
                }
                if (lastLO != LO)
                {
                        fprintf(snapshot, "$LO: 0x%08X\n", LO);
                        //printf("$LO: 0x%08X\n", LO);
                }

                if (lastPC != PC)
                {
                        fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
                        //printf( "PC: 0x%08X\n\n\n", PC);
                }


                lastHI = HI;
                lastLO = LO;
                lastPC = PC;
        }
        cycle++;
}
void iTLB_init()
{
	iTLB_entries = iPTE_entries / 4;
	iTLB = (TLB*)malloc(iTLB_entries*sizeof(TLB));
	for (int i = 0; i < iTLB_entries; i++)
	{
		iTLB[i].last_cycle = 0;
		iTLB[i].PPN = 0;
		iTLB[i].VPN = 0;
		iTLB[i].valid = 0;
	}
	iTLB_miss = 0;
	iTLB_hit = 0;
}
void iPTE_init()
{
	iPTE_entries = iDisk_size / iPage_size;
	iPTE = (PTE*)malloc(iPTE_entries * sizeof(PTE));
	for (int i = 0; i < iPTE_entries; i++)
	{
		iPTE[i].PPN = 0 ;
		iPTE[i].valid = 0;
	}
	iPTE_miss = 0;
	iPTE_hit = 0;
}
void iCache_init()
{
	iCache_entries = (iCache_size / iCache_associate_num) / 4;
	iCACHE = (CACHE**)malloc(iCache_entries * sizeof(CACHE*));
	for (int i = 0; i<iCache_entries; i++) {
		iCACHE[i] = (CACHE*)malloc(iCache_associate_num * sizeof(CACHE));
	}
	for (int i = 0; i<iCache_entries; i++) {
		for (int j = 0; j<iCache_associate_num; j++) {
			iCACHE[i][j].MRU = 0;
			iCACHE[i][j].tag = 0;
			iCACHE[i][j].valid = 0;
		}
	}
	iCACHE_hit = 0;
	iCACHE_miss = 0;
}
void iMemory_init()
{
	iMem_entries = iMem_size / iPage_size;
	iMEMORY = (MEMORY*)malloc(iMem_entries * sizeof(MEMORY));
	for (int i = 0; i<iMem_entries; i++) {
		iMEMORY[i].last_cycle = 0;
		iMEMORY[i].valid = 0;
	}
}
void dTLB_init()
{
	dTLB_entries = dPTE_entries / 4;
	dTLB = (TLB*)malloc(dTLB_entries * sizeof(TLB));
	for (int i = 0; i < dTLB_entries; i++)
	{
		dTLB[i].last_cycle = 0;
		dTLB[i].PPN = 0;
		dTLB[i].VPN = 0;
		dTLB[i].valid = 0;
	}
	dTLB_miss = 0;
	dTLB_hit = 0;
}
void dPTE_init()
{
	dPTE_entries = dDisk_size / dPage_size;
	dPTE = (PTE*)malloc(dPTE_entries * sizeof(PTE));
	for (int i = 0; i < dPTE_entries; i++)
	{
		dPTE[i].PPN = 0;
		dPTE[i].valid = 0;
	}
	dPTE_miss = 0;
	dPTE_hit = 0;
}
void dCache_init()
{
	dCache_entries =( dCache_size / dCache_associate_num )/ 4;
	dCACHE = (CACHE**)malloc(dCache_entries * sizeof(CACHE*));
	for (int i = 0; i<dCache_entries; i++) {
		dCACHE[i] = (CACHE*)malloc(dCache_associate_num * sizeof(CACHE));
	}
	for (int i = 0; i<dCache_entries; i++) {
		for (int j = 0; j<dCache_associate_num; j++) {
			dCACHE[i][j].MRU = 0;
			dCACHE[i][j].tag = 0;
			dCACHE[i][j].valid = 0;
		}
	}
	dCACHE_hit = 0;
	dCACHE_miss = 0;
}
void dMemory_init()
{
	dMem_entries = dMem_size / dPage_size;
	dMEMORY = (MEMORY*)malloc(dMem_entries * sizeof(MEMORY));
	for (int i = 0; i<dMem_entries; i++) {
		dMEMORY[i].last_cycle = 0;
		dMEMORY[i].valid = 0;
	}
}
void ICMP(int VA)
{
	IVPN = VA / iPage_size;
	iPage_offset = VA % iPage_size;
	IPPN = findITLB(IVPN);
	printf("I : cycle %d:",cycle);
	//TLB hit!
	if (IPPN != -1)
	{
		printf(" iTLB hit /");
		iTLB_hit++;
		if (findICACHE(IPPN) == 1)
		{
			iCACHE_hit++;
			printf(" iCache hit /");
		}
			
		else
		{
			iCACHE_miss++;
			updateICACHE(IPPN);
		}
	}
	else //TLB miss!!!
	{
		iTLB_miss++;
		//Check PTE
		IPPN = findIPTE(IVPN);

		if (IPPN != -1) //PTE hit!!
		{
			printf(" iPTE hit /");
			iPTE_hit++;
			updateITLB(IVPN);
			//Check CACHE
			IPPN = findITLB(IVPN);
			if (findICACHE(IPPN) == 1)
			{
				printf(" iCache hit /");
				iCACHE_hit++;
			}
			else
			{
				iCACHE_miss++;
				updateICACHE(IPPN);
			}
		}
		else  //PTE miss!!!!!!!!!!!!!!!!!!!!!
		{
			iPTE_miss++;
			updateIPTE(IVPN);
			updateITLB(IVPN);
			IPPN = findITLB(IVPN);
			if (findICACHE(IPPN) == 0) {
				printf(" iDisk hit");
				iCACHE_miss++;
				// go update CACHE
				updateICACHE(IPPN);
			}
		}
	}
	
}
void DCMP(int VA)
{
	DVPN = VA / dPage_size;
	dPage_offset = VA % dPage_size;
	DPPN = findDTLB(DVPN);
	printf(" ;");
	//TLB hit!
	if (DPPN != -1)
	{
		printf(" dTLB hit /");
		dTLB_hit++;
		if (findDCACHE(DPPN) == 1)
		{
			dCACHE_hit++;
			printf(" dCache hit /");
		}
			
		else
		{
			dCACHE_miss++;
			updateDCACHE(DPPN);
		}
	}
	else //TLB miss!!!
	{
		dTLB_miss++;
		//Check PTE
		DPPN = findDPTE(DVPN);

		if (DPPN != -1) //PTE hit!!
		{
			printf(" dPTE hit /");
			dPTE_hit++;
			updateDTLB(DVPN);
			//Check CACHE
			DPPN = findDTLB(DVPN);
			if (findDCACHE(DPPN) == 1)
			{
				printf(" dCache hit /");
				dCACHE_hit++;
			}
			else
			{
				dCACHE_miss++;
				updateDCACHE(DPPN);
			}
		}
		else  //PTE miss!!!!!!!!!!!!!!!!!!!!!
		{
			dPTE_miss++;
			updateDPTE(DVPN);
			updateDTLB(DVPN);
			DPPN = findDTLB(DVPN);
			if (findDCACHE(DPPN) == 0) {
				printf(" dDisk hit");
				dCACHE_miss++;
				// go update CACHE
				updateDCACHE(DPPN);
			}
		}
	}

}
int findITLB(int VPN)
{
	for (int i = 0; i < iTLB_entries; i++)
	{
		if (iTLB[i].VPN == VPN && iTLB[i].valid == 1 && iTLB[i].last_cycle > 0)
		{
			iTLB[i].last_cycle = cycle;
			return iTLB[i].PPN;
		}
	}
	return -1;
}
int findIPTE(int VPN)
{
	if (iPTE[VPN].valid == 1)
		return iPTE[VPN].PPN;
	else return -1;
}
void updateIPTE(int VPN)
{
	//Swap with memory!
	int PPN = 0;
	int min = INT32_MAX;
	int flag = 0;
	for (int i = 0; i<iMem_entries; i++)
	{
		if (iMEMORY[i].valid == 0) {
			PPN = i;
			flag = 1;
			break;
		}
		else {
			if (iMEMORY[i].last_cycle < min) {
				PPN = i;
				min = iMEMORY[i].last_cycle;
			}
		}
	}
	iMEMORY[PPN].valid = 1;
	iMEMORY[PPN].last_cycle = cycle;

	//PTE
	if (flag == 1) {
		iPTE[VPN].PPN = PPN;
		iPTE[VPN].valid = 1;
	}
	else {
		for (int i = 0; i<iPTE_entries; i++) {
			if (iPTE[i].PPN == PPN) {
				iPTE[i].valid = 0;
			}
		}
		iPTE[VPN].PPN = PPN;
		iPTE[VPN].valid = 1;

		for (int i = 0; i<iTLB_entries; i++) {
			if (iTLB[i].PPN == PPN) {
				iTLB[i].valid = 0;
			}
		}
		for (int j = 0; j<iPage_size; j++) {
			int PA = PPN * iPage_size + j;
			int PABlock = PA / iBlock_size;
			int index = PABlock % iCache_entries;
			int tag = PABlock / iCache_entries;
			if (iCache_associate_num == 1) {
				if (iCACHE[index][0].tag == tag) {
					iCACHE[index][0].valid = 0;
				}
			}
			else {
				for (int i = 0; i<iCache_associate_num; i++) {
					if (iCACHE[index][i].tag == tag && iCACHE[index][i].valid == 1) {
						iCACHE[index][i].valid = 0;
						iCACHE[index][i].MRU = 0;
					}
				}
			}
		}
	}
}
void updateITLB(int VPN)
{
	int min = INT32_MAX;
	int temp = 0;
	int PPN = 0;

	for (int i = 0; i < iTLB_entries; i++)
	{
		if (iTLB[i].valid == 0)
		{
			temp = i;
			break;
		}
		else
		{
			if (iTLB[i].last_cycle < min)
			{
				min = iTLB[i].last_cycle;
				temp = i;
			}
		}
	}
	PPN = iPTE[VPN].PPN;

	iTLB[temp].last_cycle = cycle;
	iTLB[temp].valid = 1;
	iTLB[temp].PPN = PPN;
	iTLB[temp].VPN = VPN;
}
inline int iCACHE_MRU(unsigned index)
{
	unsigned isAllOne = 1;
	for (int j = 0; j < iCache_associate_num; j++)
		isAllOne &= iCACHE[index][j].MRU;
	return isAllOne;
}
inline iCACHE_MRU_CLEAR(unsigned index , int i)
{
	for (int j = 0; j < iCache_associate_num; j++)
	{
		if (j != i)
			iCACHE[index][j].MRU = 0;
	}
}

int findDTLB(int VPN)
{
	for (int i = 0; i < iTLB_entries; i++)
	{
		if (dTLB[i].VPN == VPN && dTLB[i].valid == 1 && dTLB[i].last_cycle > 0)
		{
			dTLB[i].last_cycle = cycle;
			return dTLB[i].PPN;
		}
	}
	return -1;
}
int findDPTE(int VPN)
{
	if (dPTE[VPN].valid == 1)
		return dPTE[VPN].PPN;
	else return -1;
}
void updateDPTE(int VPN)
{
	//Swap with memory!
	int PPN = 0;
	int min = INT32_MAX;
	int flag = 0;
	for (int i = 0; i<dMem_entries; i++)
	{
		if (dMEMORY[i].valid == 0) {
			PPN = i;
			flag = 1;
			break;
		}
		else {
			if (dMEMORY[i].last_cycle < min) {
				PPN = i;
				min = dMEMORY[i].last_cycle;
			}
		}
	}
	dMEMORY[PPN].valid = 1;
	dMEMORY[PPN].last_cycle = cycle;

	//PTE
	if (flag == 1) {
		dPTE[VPN].PPN = PPN;
		dPTE[VPN].valid = 1;
	}
	else {
		for (int i = 0; i<dPTE_entries; i++) {
			if (dPTE[i].PPN == PPN) {
				dPTE[i].valid = 0;
			}
		}
		dPTE[VPN].PPN = PPN;
		dPTE[VPN].valid = 1;

		for (int i = 0; i<dTLB_entries; i++) {
			if (dTLB[i].PPN == PPN) {
				dTLB[i].valid = 0;
			}
		}
		for (int j = 0; j<dPage_size; j++) {
			int PA = PPN * dPage_size + j;
			int PABlock = PA / dBlock_size;
			int index = PABlock % dCache_entries;
			int tag = PABlock / dCache_entries;
			if (dCache_associate_num == 1) {
				if (dCACHE[index][0].tag == tag) {
					dCACHE[index][0].valid = 0;
				}
			}
			else {
				for (int i = 0; i<dCache_associate_num; i++) {
					if (dCACHE[index][i].tag == tag && dCACHE[index][i].valid == 1) {
						dCACHE[index][i].valid = 0;
						dCACHE[index][i].MRU = 0;
					}
				}
			}
		}
	}
}
void updateDTLB(int VPN)
{
	int min = INT32_MAX;
	int temp = 0;
	int PPN = 0;

	for (int i = 0; i < dTLB_entries; i++)
	{
		if (dTLB[i].valid == 0)
		{
			temp = i;
			break;
		}
		else
		{
			if (dTLB[i].last_cycle < min)
			{
				min = dTLB[i].last_cycle;
				temp = i;
			}
		}
	}
	PPN = dPTE[VPN].PPN;

	dTLB[temp].last_cycle = cycle;
	dTLB[temp].valid = 1;
	dTLB[temp].PPN = PPN;
	dTLB[temp].VPN = VPN;
}
inline int dCACHE_MRU(unsigned index)
{
	unsigned isAllOne = 1;
	for (int j = 0; j < dCache_associate_num; j++)
		isAllOne &= dCACHE[index][j].MRU;
	return isAllOne;
}
inline dCACHE_MRU_CLEAR(unsigned index, int i)
{
	for (int j = 0; j < dCache_associate_num; j++)
	{
		if (j != i)
			dCACHE[index][j].MRU = 0;
	}
}
int findICACHE(int PPN)
{
	int PA = PPN * iPage_size + iPage_offset;
	int PABlock = PA / iBlock_size;
	int tag = PABlock / iCache_entries;
	int index = PABlock % iCache_entries;

	if (iCache_associate_num == 1)
	{
		if (iCACHE[index][0].tag == tag && iCACHE[index][0].valid == 1)
		{
			return 1;
		}
	}
	else
	{
		for (int i = 0; i < iCache_associate_num; i++)
		{
			if (iCACHE[index][i].tag == tag&&iCACHE[index][i].valid == 1)
			{
				iCACHE[index][i].MRU = 1;

				if (iCACHE_MRU(index) == 1)
				{
					iCACHE_MRU_CLEAR(index, i);
					return 1;
				}
				return 1;
			}
		}
	}
	return 0;
}
int findDCACHE(int PPN)
{
	int PA = PPN * dPage_size + dPage_offset;
	int PABlock = PA / dBlock_size;
	int tag = PABlock / dCache_entries;
	int index = PABlock % dCache_entries;

	if (dCache_associate_num == 1)
	{
		if (dCACHE[index][0].tag == tag &&dCACHE[index][0].valid == 1)
		{
			return 1;
		}
	}
	else
	{
		for (int i = 0; i <dCache_associate_num; i++)
		{
			if (dCACHE[index][i].tag == tag&&dCACHE[index][i].valid == 1)
			{
				dCACHE[index][i].MRU = 1;
				if(dCACHE_MRU(index)==1)
				{
					dCACHE_MRU_CLEAR(index, i);
					return 1;
				}
				return 1;
			}
		}
	}
	return 0;
}
void updateICACHE(int PPN)
{
	int PA = PPN*iPage_size + iPage_offset;
	int PABlock = PA / iBlock_size;
	int index = PABlock%iCache_entries;
	int tag = PABlock / iCache_entries;
	int isVacantExist = 0;
	int VacantNum = 0;
	int firstMRUisZero = 0;
	int temp = 0;
	int flag = 0;
	if (iCache_associate_num == 1)
	{
		iCACHE[index][0].MRU = 0;
		iCACHE[index][0].tag = tag;
		iCACHE[index][0].valid = 1;
	}
	else
	{
		//find 1st invalid set
		for (int i = 0; i < iCache_associate_num; i++)
		{
			if (iCACHE[index][i].valid == 0)
			{
				isVacantExist = 1;
				VacantNum = i;
				break;
			}
		}

		//find 1st MRU==0 set
		for (int i = 0; i < iCache_associate_num; i++)
		{
			if (iCACHE[index][i].MRU == 0)
			{
				if (flag == 0)
				{
					firstMRUisZero = i;
					flag = 1;
				}
				else
				{
					flag = -1;
					break;
				}
			}
		}

		if (isVacantExist == 1)
		{
			iCACHE[index][VacantNum].MRU = 1;
			unsigned isAllOne = 1;
			for (int i = 0; i < iCache_associate_num; i++)
				isAllOne &= iCACHE[index][i].MRU;

			if (isAllOne == 1 && VacantNum == firstMRUisZero)
				for (int i = 0; i < iCache_associate_num; i++)
					if (i != firstMRUisZero)
						iCACHE[index][i].MRU = 0;

			iCACHE[index][VacantNum].tag = tag;
			iCACHE[index][VacantNum].valid = 1;
		}
		else
		{
			iCACHE[index][firstMRUisZero].MRU = 1;
			unsigned isAllOne = 1;
			for (int i = 0; i < iCache_associate_num; i++)
				isAllOne &= iCACHE[index][i].MRU;

			if (isAllOne == 1)
				for (int i = 0; i < iCache_associate_num; i++)
					if (i != firstMRUisZero)
						iCACHE[index][i].MRU = 0;

			iCACHE[index][firstMRUisZero].tag = tag;
			iCACHE[index][firstMRUisZero].valid = 1;
		}
	}
}
void updateDCACHE(int PPN)
{
	int PA = PPN*dPage_size + dPage_offset;
	int PABlock = PA / dBlock_size;
	int index = PABlock%dCache_entries;
	int tag = PABlock / dCache_entries;
	int isVacantExist = 0;
	int VacantNum = 0;
	int firstMRUisZero = 0;
	int temp = 0;
	int flag = 0;
	if (dCache_associate_num == 1)
	{
		dCACHE[index][0].MRU = 0;
		dCACHE[index][0].tag = tag;
		dCACHE[index][0].valid = 1;
	}
	else
	{
		//find 1st invalid set
		for (int i = 0; i < dCache_associate_num; i++)
		{
			if (dCACHE[index][i].valid == 0)
			{
				isVacantExist = 1;
				VacantNum = i;
				break;
			}
		}

		//find 1st MRU==0 set
		for (int i = 0; i < dCache_associate_num; i++)
		{
			if (dCACHE[index][i].MRU == 0)
			{
				if (flag == 0)
				{
					firstMRUisZero = i;
					flag = 1;
				}
				else
				{
					flag = -1;
					break;
				}
			}
		}

		if (isVacantExist == 1)
		{
			dCACHE[index][VacantNum].MRU = 1;
			unsigned isAllOne = 1;
			for (int i = 0; i < dCache_associate_num; i++)
				isAllOne &= dCACHE[index][i].MRU;

			if (isAllOne == 1 && VacantNum == firstMRUisZero)
				for (int i = 0; i < dCache_associate_num; i++)
					if (i != firstMRUisZero)
						dCACHE[index][i].MRU = 0;

			dCACHE[index][VacantNum].tag = tag;
			dCACHE[index][VacantNum].valid = 1;
		}
		else
		{
			dCACHE[index][firstMRUisZero].MRU = 1;
			unsigned isAllOne = 1;
			for (int i = 0; i < dCache_associate_num; i++)
				isAllOne &= dCACHE[index][i].MRU;

			if (isAllOne == 1)
				for (int i = 0; i < dCache_associate_num; i++)
					if (i != firstMRUisZero)
						dCACHE[index][i].MRU = 0;

			dCACHE[index][firstMRUisZero].tag = tag;
			dCACHE[index][firstMRUisZero].valid = 1;
		}
	}
}
