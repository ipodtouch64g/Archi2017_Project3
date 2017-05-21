#include "commands.h"



void R_Command(char *op) {
	findRSRTRD();
	findSHAMT();
	findFUNC();
	if (strcmp(op, "add") == 0)
	{
		//printf("0x%08x : add : $%u , $%u ,$%u\n",PC,rd,rs,rt);
		int s_rd;
		int s_rs = (int)reg[rs];
		int s_rt = (int)reg[rt];
		s_rd = s_rs + s_rt;
		detectNumOverflow(s_rs, s_rt, s_rd);
		reg[rd] = (unsigned)s_rd;
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "addu") == 0)
	{
		//printf("0x%08x : addu : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = reg[rs] + reg[rt];
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "sub") == 0)
	{
		//printf("0x%08x : sub : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		int s_rd;
		int s_rs = (int)reg[rs];
		int s_rt = (int)reg[rt];
		s_rd = s_rs - s_rt;
		detectNumOverflow(s_rs, (-1)*s_rt, s_rd);
		reg[rd] = (unsigned)s_rd;
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "and") == 0)
	{
		//printf("0x%08x : and : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = reg[rs] & reg[rt];
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "or") == 0)
	{
		//printf("0x%08x : or : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = reg[rs] | reg[rt];
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "xor") == 0)
	{
		//printf("0x%08x : xor : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = reg[rs] ^ reg[rt];
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "nor") == 0)
	{
		//printf("0x%08x : nor : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = ~(reg[rs] | reg[rt]);
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "nand") == 0)
	{
		//printf("0x%08x : nand : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = ~(reg[rs] & reg[rt]);
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "slt") == 0)
	{
		//printf("0x%08x : slt : $%u , $%u ,$%u\n", PC, rd, rs, rt);
		reg[rd] = ((int)reg[rs] < (int)reg[rt]) ? 1 : 0;
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "sll") == 0)
	{
		//printf("0x%08x : sll : $%u , $%u ,$%u\n", PC, rd, rt, shamt);
		reg[rd] = reg[rt] << shamt;
		if (rt != 0 || shamt != 0)
		{
			if (rd == 0)
				detectWriteRegZero();
		}
	}

	else if (strcmp(op, "srl") == 0)
	{
		//printf("0x%08x : srl : $%u , $%u ,$%u\n", PC, rd, rt, shamt);
		reg[rd] = reg[rt] >> shamt;
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "sra") == 0)
	{
		//printf("0x%08x : sra : $%u , $%u ,$%u\n", PC, rd, rt, shamt);
		reg[rd] = (int)reg[rt] >> shamt;
		if (rd == 0)
		{
			detectWriteRegZero();
		}
	}

	else if (strcmp(op, "jr") == 0)
	{
		//printf("0x%08x : jr : $%u\n", PC, rs);
		PC = reg[rs];
		return; //DONT need to PC+=4
	}

	else if (strcmp(op, "mult") == 0)
	{
		//printf("0x%08x : mult : $%u , $%u\n", PC, rs, rt);
		int64_t rs_64 = (int)reg[rs];
		int64_t rt_64 = (int)reg[rt];
		int64_t ans = rs_64*rt_64;
		//detectNumOverflow((int)reg[rs], (int)reg[rt],(int)HI);
		uint64_t u_ans = (uint64_t)ans;
		LO = u_ans << 32 >> 32;
		HI = u_ans >> 32;
		detectHILOWrite("mult");
	}

	else if (strcmp(op, "multu") == 0)
	{
		//printf("0x%08x : multu : $%u , $%u\n", PC, rs, rt);
		uint64_t rs_64 = reg[rs];
		uint64_t rt_64 = reg[rt];
		uint64_t ans = rs_64*rt_64;

		LO = ans << 32 >> 32;
		HI = ans >> 32;
		detectHILOWrite("mult");
	}

	else if (strcmp(op, "mfhi") == 0)
	{
		//printf("0x%08x : mfhi : $%u\n", PC, rd);
		reg[rd] = HI;
		if (rd == 0)
			detectWriteRegZero();
		detectHILOWrite("hilo");
	}

	else if (strcmp(op, "mflo") == 0)
	{
		//printf("0x%08x : mflo : $%u\n", PC, rd);
		reg[rd] = LO;
		if (rd == 0)
			detectWriteRegZero();
		detectHILOWrite("hilo");
	}

	else
	{
		halt = 1;
	}

	PC += 4; //go forward a word
}


void I_Command(char *op) {
	findRSRTRD();

	//ADDI
	if (strcmp(op, "addi") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : addi : $%u , $%u , $0x%02x\n", PC, rt, rs, immediate);
		if (rt == 0)
			detectWriteRegZero();
		int s_rt;
		int s_rs = (int)reg[rs];
		int s_imme = (int)immediate;
		s_rt = s_rs + s_imme;
		detectNumOverflow(s_rs, s_imme, s_rt);
		reg[rt] = (unsigned)s_rt;
		if (rt == 0)
			reg[rt] = 0;
	}

	//ADDIU
	//Very strange because addiu and addi both use signed immediate.
	//only difference is whether we need to check if overflow , WTF.
	else if (strcmp(op, "addiu") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : addiu : $%u , $%u , $0x%02x\n", PC, rt, rs, immediate);
		reg[rt] = reg[rs] + immediate;
		if (rt == 0)
			detectWriteRegZero();
	}

	//LW
	else if (strcmp(op, "lw") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : lw : $%u , 0x%02xC($%u)\n", PC, rt,immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(3);
		int datamsa = detectDataMisaligned(3);
		if (!memovf && !datamsa)
		{
			reg[rt] = (dMemory[dPos] << 24) | (dMemory[dPos + 1] << 16) | (dMemory[dPos + 2] << 8) | (dMemory[dPos + 3]);
		}

		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
		if (rt == 0)
			detectWriteRegZero();
	}

	else if (strcmp(op, "lh") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : lh : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(1);
		int datamsa = detectDataMisaligned(1);
		if (!memovf && !datamsa)
		{
			reg[rt] = (int16_t)((dMemory[dPos]) << 8 | (dMemory[dPos + 1]));
		}
		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
		if (rt == 0)
			detectWriteRegZero();
	}
	else if (strcmp(op, "lhu") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : lhu : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(1);
		int datamsa = detectDataMisaligned(1);
		if (!memovf && !datamsa)
		{
			reg[rt] = ((dMemory[dPos]) << 8 | (dMemory[dPos + 1]));
		}

		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
		if (rt == 0)
			detectWriteRegZero();

	}
	else if (strcmp(op, "lb") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : lb : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(0);
		int datamsa = detectDataMisaligned(0);
		if (!memovf && !datamsa)
		{
			reg[rt] = (int8_t)(dMemory[dPos]);
		}

		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
		if (rt == 0)
			detectWriteRegZero();

	}
	else if (strcmp(op, "lbu") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : lbu : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(0);
		int datamsa = detectDataMisaligned(0);
		if (!memovf && !datamsa)
		{
			reg[rt] = (dMemory[dPos]);
		}

		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
		if (rt == 0)
			detectWriteRegZero();

	}
	else if (strcmp(op, "sw") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : sw : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(3);
		int datamsa = detectDataMisaligned(3);
		if (!memovf && !datamsa)
		{
			dMemory[dPos] = reg[rt] >> 24;
			dMemory[dPos + 1] = (reg[rt] >> 16) & 0xff;
			dMemory[dPos + 2] = (reg[rt] >> 8) & 0xff;
			dMemory[dPos + 3] = (reg[rt]) & 0xff;
		}
		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
	}
	else if (strcmp(op, "sh") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : sh : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(1);
		int datamsa = detectDataMisaligned(1);
		if (!memovf && !datamsa)
		{
			dMemory[dPos] = (reg[rt] >> 8) & 0xff;
			dMemory[dPos + 1] = reg[rt];
		}
		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
	}
	else if (strcmp(op, "sb") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : sb : $%u , 0x%02xC($%u)\n", PC, rt, immediate, rs);
		dPos = reg[rs] + immediate;
		DCMP(dPos);
		int memovf = detectMemOverflow(0);
		int datamsa = detectDataMisaligned(0);
		if (!memovf && !datamsa)
		{
			dMemory[dPos] = reg[rt];
		}
		//check ovf
		int s_imme = (int)immediate;
		int s_pos = (int)reg[rs];
		detectNumOverflow(s_imme, s_pos, (int)dPos);
	}
	else if (strcmp(op, "lui") == 0)
	{
		findUnsignedIMMEDIATE();
		//printf("0x%08x : lui : $%u , 0x%02xC\n", PC, rt, immediate);
		reg[rt] = immediate << 16;
		if (rt == 0)
			detectWriteRegZero();
	}
	else if (strcmp(op, "andi") == 0)
	{
		findUnsignedIMMEDIATE();
		//printf("0x%08x : andi : $%u , $%u ,0x%02xC\n", PC, rt, rs,immediate);
		reg[rt] = reg[rs] & immediate;
		if (rt == 0)
			detectWriteRegZero();
	}
	else if (strcmp(op, "ori") == 0)
	{
		findUnsignedIMMEDIATE();
		//printf("0x%08x : ori : $%u , $%u ,0x%02xC\n", PC, rt, rs, immediate);
		reg[rt] = reg[rs] | immediate;
		if (rt == 0)
			detectWriteRegZero();
	}
	else if (strcmp(op, "nori") == 0)
	{
		findUnsignedIMMEDIATE();
		//printf("0x%08x : nori : $%u , $%u ,0x%02xC\n", PC, rt, rs, immediate);
		reg[rt] = ~(reg[rs] | immediate);
		if (rt == 0)
			detectWriteRegZero();
	}
	else if (strcmp(op, "slti") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : slti : $%u , $%u ,0x%02xC\n", PC, rt, rs, immediate);
		reg[rt] = ((int)reg[rs] < (int)immediate);
		if (rt == 0)
			detectWriteRegZero();
	}
	else if (strcmp(op, "beq") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : beq : $%u , $%u ,0x%02xC\n", PC, rs, rt, immediate);
		if (reg[rs] == reg[rt])
		{
			PC += (4 + (immediate << 2));
			return;
		}
	}
	else if (strcmp(op, "bne") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : bne : $%u , $%u ,0x%02xC\n", PC, rs, rt, immediate);
		if (reg[rs] != reg[rt])
		{
			PC += (4 + (immediate << 2));
			return;
		}
	}
	else if (strcmp(op, "bgtz") == 0)
	{
		findSignedIMMEDIATE();
		//printf("0x%08x : bgtz : $%u , 0x%02xC\n", PC, rs, immediate);
		if ((int)reg[rs] > 0)
		{
			PC += (4 + (immediate << 2));
			return;
		}
	}
	else
	{
		halt = 1;
	}

	PC += 4;
}

void J_Command(char *op) {
	findADDR();
	if (strcmp(op, "j") == 0)
	{
		//printf("0x%08x : j : 0x%08xC\n", PC, address);
		PC = (((PC + 4) >> 28 << 28) | (address << 2));
	}
	else if (strcmp(op, "jal") == 0)
	{
		//printf("0x%08x : jal : 0x%08xC\n", PC, address);
		reg[31] = PC + 4;
		PC = (((PC + 4) >> 28 << 28) | (address << 2));
	}
	else
	{
		halt = 1;
	}
}





//detect error
void detectWriteRegZero()
{
	writeRegZero = 1;
	reg[0] = 0;
}

void detectNumOverflow(int a, int b, int c)  //d = 1 is mul
{
	if ((a > 0 && b > 0 && c <= 0) || (a < 0 && b < 0 && c >= 0))
		numOverflow = 1;


}

void detectHILOWrite(char* op)
{
	if (strcmp(op, "mult") == 0)
	{
		if (toggledMULT == 1 && toggledHILO == 1) //NO HILO error , turn off HILO toggle now
		{
			toggledHILO = 0;
		}
		else if (toggledMULT == 1 && toggledHILO == 0) //HILO error!!
		{
			HILOOverWrite = 1;
		}
		else //first MULT in a row,turn on MULT toggle now
		{
			toggledMULT = 1;
		}
	}
	else //op == hi lo
	{
		toggledHILO = 1;
	}
}

int detectMemOverflow(int len_byte)
{
	if (dPos + len_byte >= MEM_MAX_SIZE || dPos >= MEM_MAX_SIZE)
	{
		memOverflow = 1;
		halt = 1;
		return 1;
	}
	else
		return 0;
}

int detectDataMisaligned(int len_byte)
{
	if (dPos % (len_byte + 1) != 0)
	{
		dataMisaligned = 1;
		halt = 1;
		return 1;
	}
	else
		return 0;
}


// find
void findOP()
{
	op = iMemory[PC];
	op = op >> 2 << 26 >> 26; //0x000000??
}

void findRSRTRD()
{
	//RD
	rd = iMemory[PC + 2];
	rd = rd >> 3;
	//RT
	rt = iMemory[PC + 1];
	rt = rt << 27 >> 27;
	//RS
	rs = ((unsigned)iMemory[PC] << 30 >> 27) | ((unsigned)iMemory[PC + 1] << 24 >> 29);
}

void findFUNC()
{
	func = iMemory[PC + 3];
	func = func << 26 >> 26; //find 6 lsb
}

void findSHAMT()
{
	shamt = ((unsigned)iMemory[PC + 2] << 29 >> 27) | ((unsigned)iMemory[PC + 3] >> 6);
}

void findUnsignedIMMEDIATE()
{
	immediate = ((unsigned)iMemory[PC + 2] << 8) | ((unsigned)iMemory[PC + 3]);
}

//Tricky part to get 16bit immediate SIGN EXTENSION to 32bit immediate
void findSignedIMMEDIATE()
{
	immediate = (int16_t)(((unsigned)iMemory[PC + 2] << 8) | ((unsigned)iMemory[PC + 3]));
}

void findADDR()
{
	address = ((unsigned)iMemory[PC + 3]) | ((unsigned)iMemory[PC + 2] << 8) | ((unsigned)iMemory[PC + 1] << 16) | ((unsigned)iMemory[PC] << 30 >> 6);
}
