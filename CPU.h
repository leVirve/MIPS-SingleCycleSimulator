#ifndef __CPU_H__
#define __CPU_H__

#include "env.h"
#include "Memory.h"
#include "struct.h"

class CPU {

private:
	UINT32 pc;
	UINT32 reg[32];
	Memory* memory;

public:
	CPU();
	void setMemory(Memory*);
	void setPC(UINT32);
	UINT32 PC();
	UINT32 fetch(UINT32);
	Operand decode(UINT32);
	bool exec(Operand);
	
	UINT32 getReg(int);
	int setReg(int, UINT32);
	UINT32 getPC();

	void R_decoder(Operand& op, UINT32& instruction);
	void I_decoder(Operand& op, UINT32& instruction);
	void J_decoder(Operand& op, UINT32& instruction);

	void R_exec(Operand instr);
	void I_exec(Operand instr);
	void J_exec(Operand instr);

	void OpAdd(Operand);
	void OpSub(Operand);
	void OpAnd(Operand);
	void OpOr(Operand);
	void OpXor(Operand);
	void OpNor(Operand);
	void OpNand(Operand);
	void OpSlt(Operand);
	void OpSll(Operand);
	void OpSrl(Operand);
	void OpSra(Operand);
	void OpJr(Operand);
	
	void OpAddi(Operand);
	void OpLw(Operand);
	void OpLh(Operand);
	void OpLhu(Operand);
	void OpLb(Operand);
	void OpLbu(Operand);
	void OpSw(Operand);
	void OpSh(Operand);
	void OpSb(Operand);
	void OpLui(Operand);
	void OpAndi(Operand);
	void OpOri(Operand);
	void OpNori(Operand);
	void OpSlti(Operand);
	void OpBeq(Operand);
	void OpBne(Operand);
	
	void OpJ(Operand);
	void OpJal(Operand);
	void OpHalt(Operand);
};

#endif
