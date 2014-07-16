#include "Memory.hpp"
#include "RegisterFile.hpp"
#include "Stage.h"
#include "Component.hpp"
#include "Register.h"

#ifdef _DEBUG
#include "MIPS_to_ASM.h"
#endif

class Stage {

public:
	Stage() {};
	virtual bool execute() = 0;
	virtual void toString() = 0;
};

class FetchStage : public Stage {

private:

	Memory& _memory;
	UINT32 _pc;

	UINT32& isBranch;
	UINT32& isJump;

	UINT32& inBranchPC;
	UINT32& inJumpPC;

	UINT32& outInstruction;
	UINT32& outPC;

public:

	FetchStage():
		_memory  (Memory::getInstructionMemory()),
		_pc      (registers["PC"]),
		outInstruction (registers["IF/ID Instruction"]),
		outPC          (registers["IF/ID PC"]),
		inBranchPC (registers["ID/IF BranchPC"]),
		inJumpPC   (registers["ID/IF JimpPC"]),
		isBranch (registers["IF Branch"]),
		isJump   (registers["IF Jump"])
		{
	}

	bool execute() {

		UINT32 raw = _memory.getInstruction(_pc);
		
		outInstruction =
			(raw >> 24) & 0xff |
			(raw >> 8) & 0xff00 |
			(raw << 8) & 0xff0000 |
			(raw << 24) & 0xff000000;
		
		_pc += 4;

		if(isBranch) outPC = inBranchPC;
		else if(isJump) outPC = inJumpPC;
		else outPC = _pc;

		return false;
	}

	void toString() {
		printf("IF : %s\n", translate_mips_to_asm(outInstruction));
	}

};

class DecodeStage : public Stage{

private:

	RegisterFile& _reg;
	ControlUnit&  _control;
	UINT32&       _isEq;
	UINT32&       _Op;
	UINT32&       _func;

	UINT32& inInstruction;
	UINT32& inPC;
	UINT32& inRd;
	UINT32& inData;

	UINT32& outInstruction;
	UINT32& outReadData1;
	UINT32& outReadData2;
	UINT32& outImmediate;
	UINT32& outRs;
	UINT32& outRt;
	UINT32& outRd;
	UINT32& outBranchPC;
	UINT32& outJumpPC;

	UINT32& RegWrite;

public:

	DecodeStage() :
		_reg     (RegisterFile::getInstance()),
		_control (ControlUnit::getInstance()),
		_isEq    (registers["ID Eq"]),
		_Op      (registers["ID Op"]),
		_func    (registers["ID func"]),
		inInstruction (registers["IF/ID Instruction"]),
		inPC          (registers["IF/ID PC"]),
		inRd    (registers["WB/ID Rd"]),
		inData  (registers["WB/ID Data"]),
		outInstruction (registers["ID/EX Instruction"]),
		outReadData1  (registers["ID/EX ReadData1"]),
		outReadData2  (registers["ID/EX ReadData2"]),
		outImmediate  (registers["ID/EX Immediate"]),
		outRs         (registers["ID/EX Rs"]),
		outRt         (registers["ID/EX Rt"]),
		outRd         (registers["ID/EX Rd"]),
		outBranchPC   (registers["ID/IF BranchPC"]),
		outJumpPC     (registers["ID/IF JumpPC"]),
		RegWrite (registers["WB/ID RegWrite"])
		{
	}

	bool execute() {

		if(RegWrite) _reg.setRegister(inRd, inData);

		/* Write\Load */

		UINT32 _Rs    = (inInstruction >> 21) & 0x1F;
		UINT32 _Rt    = (inInstruction >> 16) & 0x1F;
		UINT32 _Rd    = (inInstruction >> 11) & 0x1F;
		UINT32 _f     = (inInstruction >>  6) & 0x1F;
		UINT32 _immediate  = (signed short)(inInstruction & 0xFFFF);
		UINT32 _address  = inInstruction & 0x03ffffff;
		
		outInstruction = inInstruction;
		outReadData1 = _reg.getRegister(_Rs);
		outReadData2 = _reg.getRegister(_Rt);
		outImmediate = _immediate;
		outRs = _Rs;
		outRt = _Rt;
		outRd = _Rd;
		outBranchPC = inPC + (_immediate << 2);
		outJumpPC = (inPC & 0xf0000000)  | (_address << 2);

		_Op   = (inInstruction >> 26) & 0x3F;
		_func = _f;
		_isEq = (outReadData1 == outReadData2);
		_control.execute();

		if (_Op == 0x0 && _func == 0x8) outJumpPC = outReadData1;
		return false;
	}

	void toString() {
		printf("ID : %s\n", translate_mips_to_asm(inInstruction));
	}

};

class ExecuteStage : public Stage{

private:

	UINT32& _RegDst;
	UINT32& _ALUop;
	UINT32& _ALUSrc;

	UINT32& inInstruction;
	UINT32& inReadData1;
	UINT32& inReadData2;
	UINT32& inImmediate;
	UINT32& inRs;
	UINT32& inRt;
	UINT32& inRd;

	UINT32& outALUout;
	UINT32& outWriteData;
	UINT32& outRd;

	UINT32 op;

public:

	ExecuteStage() :
		_RegDst   (registers["ID/EX RegDst"]),
		_ALUop    (registers["ID/EX ALUop"]),
		_ALUSrc   (registers["ID/EX ALUSrc"]),
		inInstruction (registers["ID/EX Instruction"]),
		inReadData1   (registers["ID/EX ReadData1"]),
		inReadData2   (registers["ID/EX ReadData2"]),
		inImmediate   (registers["ID/EX Immediate"]),
		inRs          (registers["ID/EX Rs"]),
		inRt          (registers["ID/EX Rt"]),
		inRd          (registers["ID/EX Rd"]),
		outALUout   (registers["EX/MEM ALUout"]),
		outWriteData(registers["EX/MEM WriteData"]),
		outRd       (registers["EX/MEM Rd"])
		{
	}

	bool execute() {
		registers["EX/MEM Instruction"] = inInstruction;
		op = registers["EX Op"] = registers["ID Op"];

		UINT32 A = inReadData1;
		UINT32 B = _ALUSrc ? inImmediate : inReadData2;
		
		outALUout = ALU(_ALUop, A, B);
		outWriteData = inReadData2;
		outRd = _RegDst == 2 ? 0x11111 : _RegDst == 0 ? inRt : inRd;

		registers["EX/MEM MemRead"]  = registers["ID/EX MemRead"];
		registers["EX/MEM MemWrite"] = registers["ID/EX MemWrite"];
		registers["EX/MEM RegWrite"] = registers["ID/EX RegWrite"];
		registers["EX/MEM MemToReg"] = registers["ID/EX MemToReg"];
		registers["MEM Op"] = registers["EX Op"];
	
		return false;
	}

	int ALU(UINT32 op, UINT32 A, UINT32 B) {

		UINT32 shamt = (inInstruction >> 6) & 0x1F;
		
		switch(op) {
			case ADD:  return   A + B;
			case SUB:  return   A - B;
			case AND:  return   A & B;
			case  OR:  return   A | B;
			case XOR:  return   A ^ B;
			case NOR:  return ~(A | B);
			case NAND: return ~(A & B);
			case SLT:  return  (A < B);
			case SLL:  return   A << shamt;
			case SRL:  return   A >> shamt;
			case SRA:  return   A - B;
		}
		return 0;
	}

	void toString() {
		printf("EX : %s\n", translate_mips_to_asm(inInstruction));
	}

};

class MemoryAccessStage : public Stage{

private:

	Memory& _memory;
	UINT32& _MemRead;
	UINT32& _MemWrite;

	UINT32& inALUout;
	UINT32& inWriteData;
	UINT32& inRd;

	UINT32& outMDR;
	UINT32& outALUout;

	UINT32 op;

public:

	MemoryAccessStage() :
		_memory   (Memory::getDataMemory()),
		_MemRead  (registers["EX/MEM MemRead"]),
		_MemWrite (registers["EX/MEM MemWrite"]),
		inALUout   (registers["EX/MEM ALUout"]),
		inWriteData(registers["EX/MEM WriteData"]),
		inRd       (registers["EX/MEM Rd"]),
		outMDR       (registers["MEM/WB MDR"]),
		outALUout    (registers["MEM/WB ALUout"])
		{
	}

	bool execute() {
		op = registers["MEM Op"];
		registers["MEM/WB Instruction"] = registers["EX/MEM Instruction"];
		if(_MemWrite) {
			switch (op) {
				// load
				case 0x23:
					outMDR = _memory.getWord(inALUout);
					break;
				case 0x21:
					outMDR = (signed short)(_memory.getHalfWord(inALUout));
					break;
				case 0x25:
					outMDR = _memory.getHalfWord(inALUout);
					break;
				case 0x20:
					outMDR = (signed char)(_memory.getByte(inALUout));
					break;
				case 0x24:
					outMDR = _memory.getByte(inALUout);
					break;
			}
		}

		if(_MemRead) {
			switch(op) {
				case 0x2B:
					_memory.saveWord(inALUout, inWriteData);
					break;
				case 0x29:
					_memory.saveHalfWord(inALUout, inWriteData);
					break;
				case 0x28:
					_memory.saveByte(inALUout, inWriteData);
					break;
			}
		}

		inALUout = outALUout;
		registers["MEM/WB RegDst"]   = registers["EX/MEM RegDst"];
		registers["MEM/WB MemToReg"] = registers["EX/MEM MemToReg"];
		registers["MEM/WB RegWrite"] = registers["EX/MEM RegWrite"];
		registers["WB Op"] = registers["MEM Op"];
	
		return false;
	}

	void toString() {
		printf("MEM: %s\n", translate_mips_to_asm(registers["EX/MEM Instruction"]));
	}

};

class WriteBackStage : public Stage{

private:

	UINT32& _MemToReg;

	UINT32& inMDR;
	UINT32& inALUout;

	UINT32& outData;

	UINT32 op;

public:

	WriteBackStage() :
		_MemToReg  (registers["MEM/WB MemToReg"]),
		inMDR       (registers["MEM/WB MDR"]),
		inALUout    (registers["MEM/WB ALUout"]),
		outData     (registers["WB/ID Data"])
		{
	}

	bool execute() {
		registers["WB/ID Instruction"] = registers["MEM/WB Instruction"];
		op = registers["WB Op"];

		outData = _MemToReg ? inMDR : inALUout;
		
		registers["MEM/WB RegDst"]   = registers["WB/ID RegDst"];
		registers["MEM/WB RegWrite"] = registers["WB/ID RegWrite"];

		return false;
	}

	void toString() {
		printf("WB : %s\n", translate_mips_to_asm(registers["MEM/WB Instruction"]));
	}

};