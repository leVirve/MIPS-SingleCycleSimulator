#include "Register.h"

const int X = 2;
enum ALUop { ADD, SUB, AND, NAND, OR, XOR, NOR, SLT, SLL, SRL, SRA };

class ControlUnit {
	
private: 
	
	UINT32& inOp;
	UINT32& inFunc;
	UINT32& isEq;

	UINT32& osRegDst;
	UINT32& osJump;
	UINT32& osBranch;
	UINT32& osMemRead;
	UINT32& osMemToReg;
	UINT32& osALUop;
	UINT32& osMemWrite;
	UINT32& osALUSrc;
	UINT32& osRegWrite;

	// singleton
	ControlUnit() :
		inOp       (registers["ID Op"]),
		inFunc     (registers["ID func"]),
		isEq       (registers["ID Eq"]),
		osRegDst   (registers["ID/EX Rd"]),
		osALUop    (registers["ID/EX ALUop"]),
		osALUSrc   (registers["ID/EX ALUSrc"]),
		osMemRead  (registers["ID/EX MemRead"]),
		osMemWrite (registers["ID/EX MemWrite"]),
		osRegWrite (registers["ID/EX RegWrite"]),
		osMemToReg (registers["ID/EX MemToReg"]),
		osBranch   (registers["IF Branch"]),
		osJump     (registers["IF Jump"])
		{
	}

public:

	static ControlUnit& getInstance() {
		static ControlUnit instance;
		return instance;
	}

	void execute() {
		switch(inOp) {
			case 0x0: // R-type
				osRegDst   = 1;
				osALUSrc   = 0;
				osMemToReg = 0;
				osRegWrite = 1;
				osMemRead  = 0;
				osMemWrite = 0;
				osBranch   = 0;
				osJump     = 0;
				switch(inFunc) {
					case 0x20: osALUop = ADD;  break;
					case 0x22: osALUop = SUB;  break;
					case 0x24: osALUop = AND;  break;
					case 0x25: osALUop = OR;   break;
					case 0x26: osALUop = XOR;  break;
					case 0x27: osALUop = NOR;  break;
					case 0x28: osALUop = NAND; break;
					case 0x2A: osALUop = SLT;  break;
					case 0x00: osALUop = SLL;  break;
					case 0x02: osALUop = SRL;  break;
					case 0x03: osALUop = SRA;  break;
					case 0x08: osRegDst = 2;  break;
				}
				break;
			
			case 0x23: case 0x21: case 0x25: case 0x20: case 0x24: // load
				osRegDst   = 0;
				osALUSrc   = 1;
				osMemToReg = 1;
				osRegWrite = 1;
				osMemRead  = 1;
				osMemWrite = 0;
				osBranch   = 0;
				osJump     = 0;
				osALUop    = ADD;
				break;

			case 0x2B: case 0x29: case 0x28: // save 
				osRegDst   = X;
				osALUSrc   = 1;
				osMemToReg = X;
				osRegWrite = 0;
				osMemRead  = 0;
				osMemWrite = 1;
				osBranch   = 0;
				osJump     = 0;
				osALUop    = ADD;
				break;

			case 0x08: case 0x0A: case 0x0C: case 0x0D: case 0x0E: case 0x0F: // immidiate
				osRegDst   = 0;
				osALUSrc   = 1;
				osMemToReg = 0;
				osRegWrite = 1;
				osMemRead  = 0;
				osMemWrite = 0;
				osBranch   = 0;
				osJump     = 0;

				if(inOp == 0x08) osALUop = ADD;
				else if(inOp == 0x0A) osALUop = SLT;
				else if(inOp == 0x0C) osALUop = AND;
				else if(inOp == 0x0D) osALUop = OR;
				else if(inOp == 0x0E) osALUop = NOR;
				else if(inOp == 0x0F) osALUop = SLL;

				break;

			case 0x04: case 0x05: // branch
				osRegDst   = X;
				osALUSrc   = X;
				osMemToReg = X;
				osRegWrite = 0;
				osMemRead  = X;
				osMemWrite = 0;
				osBranch   = isEq ^ (inOp & 1);
				osJump     = 0;
				osALUop    = X;
				break;

			case 0x02: case 0x03: // jump
				osRegDst   = X;
				osALUSrc   = X;
				osMemToReg = X;
				osRegWrite = 0;
				osMemRead  = X;
				osMemWrite = 0;
				osBranch   = 0;
				osJump     = 1;
				osALUop    = X;
				break;
		}
	}

private:

};
