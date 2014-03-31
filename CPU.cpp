#include "CPU.h"

CPU::CPU()
{
	memset(reg, 0, sizeof(reg));
}

void CPU::setPC(UINT32 pc)
{
	this->pc = pc;
}

UINT32 CPU::PC()
{
	UINT32 current_pc = pc;
	pc = pc + 4;
	return current_pc;
}

void CPU::setMemory(Memory* _memory)
{
	this->memory = _memory;
}

/*
 * Fetch Stage:
 * - Convert 4 Byte from memory $(x) into Little Endian $(_instruction)
 * - return $(_instruction)
 */
UINT32 CPU::fetch(UINT32 x)
{
	UINT32 _instruction =
		(x >> 24) & 0xff |
		(x >> 8) & 0xff00 |
		(x << 8) & 0xff0000 |
		(x << 24) & 0xff000000;
	printf("Little Endian IR: %08x\n", _instruction);//
	return _instruction;
}

/*
* Decode Stage:
* - decode instructions into R, I, J - Type structures
* - return Operand Type - $(instruction)
*/
Operand CPU::decode(UINT32 instruction)
{
	Operand _operand;
	UINT32 _type = instruction >> (32 - 6) & 0x3f;
	printf("type: %02x\n", _type);
	switch (_type) {
		case 0x0:
			_operand.tpye = R_TYPE;
			_operand.instruction.R.opcode = _type;
			R_decoder(_operand, instruction);
			break;
		case 0x2: case 0x3:
			_operand.tpye = J_TYPE;
			_operand.instruction.J.opcode = _type;
			J_decoder(_operand, instruction);
			break;
		case 0x3f:
			_operand.tpye = HALT;
			break;
		default:
			_operand.tpye = I_TYPE;
			_operand.instruction.I.opcode = _type;
			I_decoder(_operand, instruction);
			break;
	}
	return _operand;
}

/*
* Executation Stage:
* - 
* - return  $(is_halt)
*/
bool CPU::exec(Operand instr)
{
	try {
		char _type = instr.tpye;
		switch (_type) {
		case R_TYPE:
			R_exec(instr);
			break;
		case I_TYPE:
			I_exec(instr);
			break;
		case J_TYPE:
			J_exec(instr);
			break;
		case HALT:
			return true;
			break;
		}
	} catch (char* e) {

	}
	return false;
}

UINT32 CPU::getReg(int idx)
{
	// check
	return reg[idx];
}

int CPU::setReg(int targ, UINT32 content)
{
	// check for REG$0
	if (targ == 0) throw "Write $0 error";
	reg[targ] = content;
	return 0;
}

UINT32 CPU::getPC()
{
	return pc;
}

void CPU::R_decoder(Operand& op, UINT32& instruction)
{
	op.instruction.R.rs = (instruction >> 21) & 0x1f;
	op.instruction.R.rt = (instruction >> 16) & 0x1f;
	op.instruction.R.rd = (instruction >> 11) & 0x1f;
	op.instruction.R.shamt = (instruction >> 6) & 0x1f;
	op.instruction.R.func = instruction & 0x3f;
}

void CPU::I_decoder(Operand& op, UINT32& instruction)
{
	op.instruction.I.rs = (instruction >> 21) & 0x1f;
	op.instruction.I.rt = (instruction >> 16) & 0x1f;
	op.instruction.I.immediate = instruction & 0xffff;
}

void CPU::J_decoder(Operand& op, UINT32& instruction)
{
	op.instruction.J.address = instruction & 0x3ffffff;
}

void CPU::R_exec(Operand instr)
{
	switch (instr.instruction.R.func) {
		case 0x20:
			OpAdd(instr);
			break;
		case 0x22:
			OpSub(instr);
			break;
		case 0x24:
			OpAnd(instr);
			break;
		case 0x25:
			OpOr(instr);
			break;
		case 0x26:
			OpXor(instr);
			break;
		case 0x27:
			OpNor(instr);
			break;
		case 0x28:
			OpNand(instr);
			break;
		case 0x2A:
			OpSlt(instr);
			break;
	}
}

void CPU::I_exec(Operand instr)
{
	switch (instr.instruction.I.opcode) {
		case 0x8:
			OpAddi(instr);
			break;
		case 0xF:
			OpLui(instr);
			break;
		case 0xC:
			OpAndi(instr);
			break;
		case 0xD:
			OpOri(instr);
			break;
		case 0xE:
			OpNori(instr);
			break;
		case 0xA:
			OpSlti(instr);
			break;
		case 0x4:
			OpBeq(instr);
			break;
		case 0x5:
			OpBne(instr);
			break;
	}
}

void CPU::J_exec(Operand instr)
{
	switch (instr.instruction.I.opcode) {
		case 0x02
			Opj(instr);
			break;
		case 0x03
			Opjal(instr);
			break;
	}
}


/////

bool isOverflow(UINT32 org, UINT32 now);

UINT32 SignExtImm(UINT32 t);


void CPU::OpAdd(Operand op)
{
	// R[rd] = R[rs] + R[rt]
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	UINT32 val = rs + rt;
	if (isOverflow(rs, val)) 
		throw "Number overflow";
	this->setReg(op.instruction.R.rd, val);
}

void CPU::OpSub(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	UINT32 val = rs - rt;
	if (isOverflow(rs, val))
		throw "Number overflow";
	this->setReg(op.instruction.R.rd, val);
}
void CPU::OpAnd(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, rs&rt);
}
void CPU::OpOr(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, rs | rt);
}
void CPU::OpXor(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, rs^rt);
}
void CPU::OpNor(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, !(rs | rt));
}
void CPU::OpNand(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, !(rs&rt));
}
void CPU::OpSlt(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.R.rs);
	UINT32 rt = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, rs<rt);
}
void CPU::OpAddi(Operand op)
{
	// rt = rs + c;
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 c = SignExtImm(op.instruction.I.immediate);
	UINT32 val = rs + c;
	if (isOverflow(rs, val))
		throw "Number overflow";
	this->setReg(op.instruction.I.rt, val);
}
void CPU::OpLui(Operand op)
{
	UINT32 c = op.instruction.I.immediate;
	c =	c << 16;
	this->setReg(op.instruction.I.rt, c);
}
void CPU::OpAndi(Operand op)
{
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 c = op.instruction.I.immediate;
	rs &= c;
	this->setReg(op.instruction.I.rt, rs);
}
void CPU::OpOri(Operand op)
{
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 c = op.instruction.I.immediate;
	rs |= c;
	this->setReg(op.instruction.I.rt, rs);
}
void CPU::OpNori(Operand op)
{
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 c = op.instruction.I.immediate;
	rs = ~(rs | c);
	this->setReg(op.instruction.I.rt, rs);
}
void CPU::OpSlti(Operand op)
{
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 c = SignExtImm(op.instruction.I.immediate);
	this->setReg(op.instruction.I.rt, (rs<c));
}
void CPU::OpBeq(Operand op)
{
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 rt = this->getReg(op.instruction.I.rt);
	UINT32 c = SignExtImm(op.instruction.I.immediate);
	if (rs == rt) this->setPC(this->pc + 4 + 4 * c);
}
void CPU::OpBne(Operand op)
{
	UINT32 rs = this->getReg(op.instruction.I.rs);
	UINT32 rt = this->getReg(op.instruction.I.rt);
	UINT32 c = SignExtImm(op.instruction.I.immediate);
	if (rs != rt) this->setPC(this->pc + 4 + 4 * c);
}

bool isOverflow(UINT32 org, UINT32 now)
{
	//
	return false;
}

UINT32 SignExtImm(UINT32 t)
{
	//
	return t;
}