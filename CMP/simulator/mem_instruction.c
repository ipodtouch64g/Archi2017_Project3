#include "mem_instruction.h"

void MachineCodeToCommands()
{
	if (PC > 1024) {
		halt = 1;
		return;
	}
		
								findOP();
								switch (op)
								{
								case 0: //R
																findFUNC();
																switch (func)
																{
																case 0x20:
																								R_Command("add"); break;
																case 0x21:
																								R_Command("addu"); break;
																case 0x22:
																								R_Command("sub"); break;
																case 0x24:
																								R_Command("and"); break;
																case 0x25:
																								R_Command("or"); break;
																case 0x26:
																								R_Command("xor"); break;
																case 0x27:
																								R_Command("nor"); break;
																case 0x28:
																								R_Command("nand"); break;
																case 0x2A:
																								R_Command("slt"); break;
																case 0x00:
																								R_Command("sll"); break;
																case 0x02:
																								R_Command("srl"); break;
																case 0x03:
																								R_Command("sra"); break;
																case 0x08:
																								R_Command("jr"); break;
																case 0x18:
																								R_Command("mult"); break;
																case 0x19:
																								R_Command("multu"); break;
																case 0x10:
																								R_Command("mfhi"); break;
																case 0x12:
																								R_Command("mflo"); break;
																default:
																								break;
																}
																break;
								case 0x08:
																I_Command("addi"); break;
								case 0x09:
																I_Command("addiu"); break;
								case 0x23:
																I_Command("lw"); break;
								case 0x21:
																I_Command("lh"); break;
								case 0x25:
																I_Command("lhu"); break;
								case 0x20:
																I_Command("lb"); break;
								case 0x24:
																I_Command("lbu"); break;
								case 0x2b:
																I_Command("sw"); break;
								case 0x29:
																I_Command("sh"); break;
								case 0x28:
																I_Command("sb"); break;
								case 0x0f:
																I_Command("lui"); break;
								case 0x0c:
																I_Command("andi"); break;
								case 0x0d:
																I_Command("ori"); break;
								case 0x0e:
																I_Command("nori"); break;
								case 0x0a:
																I_Command("slti"); break;
								case 0x04:
																I_Command("beq"); break;
								case 0x05:
																I_Command("bne"); break;
								case 0x07:
																I_Command("bgtz"); break;
								case 0x02:
																J_Command("j"); break;
								case 0x03:
																J_Command("jal"); break;
								case 0x3f:
																halt=1; break;
								default:
																printf("illegal instruction found at 0x%08X\n", PC); halt = 1; break;
								}

}
