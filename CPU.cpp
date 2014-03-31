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
 * IF:Instruction Fetch
 * - Convert 4 Byte from memory $(x) into Little Endian $_instruction
 *  @param (start address)
 *  @return $_instruction
 */
UINT32 CPU::fetch(UINT32 x)
{
	UINT32 _instruction =
		(x >> 24) & 0xff |
		(x >> 8) & 0xff00 |
		(x << 8) & 0xff0000 |
		(x << 24) & 0xff000000;
#ifdef _DEBUG
	printf("Little Endian IR: %08x\n", _instruction);
#endif
	return _instruction;
}

/*
* Decode Stage:
* - decode instructions into R, I, J - Type structures
*  @param unsigned int 32-bit $instruction
*  @return (Operand_Type) $_oprand
*/
Operand CPU::decode(UINT32 instruction)
{
	Operand _operand;
	// distinguish instructions from R, I, J type
	UINT32 _type = instruction >> (32 - 6) & 0x3f;
#ifdef _DEBUG
	printf("type: %02x\n", _type);
#endif
	switch (_type) {
		case 0x0: // R-type
			_operand.tpye = R_TYPE;
			_operand.instruction.R.opcode = _type;
			R_decoder(_operand, instruction);
			break;
		case 0x2: case 0x3: // J-type
			_operand.tpye = J_TYPE;
			_operand.instruction.J.opcode = _type;
			J_decoder(_operand, instruction);
			break;
		case 0x3f:
			_operand.tpye = HALT;
			break;
		default: // I-type
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

/*
 * @param  (16-bit immediate field) t
 * @return (32-bit) ext
 */
UINT32 SignExtImm(UINT32 t)
{
	if ((t >> 15) == 0x0)
		return t & 0xffff;
	else
		return t | 0xffff0000;
}

UINT32 ZeroExtImm(UINT32 t)
{
	return t & 0xffff;
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
		case 0x00:
			OpSll(instr);
			break;
		case 0x02:
			OpSrl(instr);
			break;
		case 0x03:
			OpSra(instr);
			break;
		case 0x08:
			OpJr(instr);
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
		case 0x02:
			OpJ(instr);
			break;
		case 0x03:
			OpJal(instr);
			break;
	}
}


/*
 * @param addends :a, b
 * @return isOverflow ? true : false
 */
bool isOverflow(UINT32 a, UINT32 b)
{
	int sum = a + b;
	// if signs of a, b are the same 
	if ( ((a >> 31) ^ (b >> 31)) == 0)
		if ((a >> 31) ^ (sum >> 31)) return true;
	return false;
}

/* R - Type */
void CPU::OpAdd(Operand op)
{
	// R[rd] = R[rs] + R[rt]
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 val = s + t;
	if (isOverflow(s, t)) throw "Number overflow";
	this->setReg(op.instruction.R.rd, val);
}

void CPU::OpSub(Operand op)
{
	// R[rd] = R[rs] - R[rt]
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 val = s + (~t+1);
	if (isOverflow(s, t)) throw "Number overflow";
	this->setReg(op.instruction.R.rd, val);
}
void CPU::OpAnd(Operand op)
{
	// R[rd] = R[rs] & R[rt]
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, s & t);
}
void CPU::OpOr(Operand op)
{
	// R[rd] = R[rs] | R[rt]
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, s | t);
}
void CPU::OpXor(Operand op)
{
	// R[rd] = R[rs] ^ R[rt]
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, s ^ t);
}
void CPU::OpNor(Operand op)
{
	// R[rd] = ~(R[rs] | R[rt])
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, ~(s | t));
}
void CPU::OpNand(Operand op)
{
	// R[rd] = ~(R[rs] & R[rt])
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, ~(s & t));
}
void CPU::OpSlt(Operand op)
{
	// R[rd] = R[rs] < R[rt] ? 1 : 0
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	this->setReg(op.instruction.R.rd, s < t ? 1 : 0);
}

void CPU::OpSll(Operand op)
{
	// R[rd] = R[rt] << shamt
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 shamt = op.instruction.R.shamt;
	this->setReg(op.instruction.R.rd, t << shamt);
}

void CPU::OpSrl(Operand op)
{
	// R[rd] = R[rt] >> shamt
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 shamt = op.instruction.R.shamt;
	this->setReg(op.instruction.R.rd, t >> shamt);
}

/* I-type */
void CPU::OpAddi(Operand op)
{
	// R[rt] = R[rs] + immediate(signed)
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	UINT32 val = s + imm;
	if (isOverflow(s, imm)) throw "Number overflow";
	this->setReg(op.instruction.I.rt, val);
}

void CPU::OpLw(Operand op)
{
	// R[rt] = 4 bytes from Memory[R[rs] + C(signed)]
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	this->setReg(op.instruction.I.rt, this->memory->getWord(s + imm));
}

void CPU::OpLh(Operand op)
{
	// R[rt] = 2 bytes from Memory[R[rs] + C(signed)], signed
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	this->setReg(op.instruction.I.rt, this->memory->getHalfWord(s + imm));
}

void CPU::OpLhu(Operand op)
{
	// R[rt] = 2 bytes from Memory[R[rs] + C(signed)], unsigned
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	this->setReg(op.instruction.I.rt, SignExtImm(this->memory->getHalfWord(s + imm)));
}

void CPU::OpLb(Operand op)
{
	// R[rt] = 2 bytes from Memory[R[rs] + C(signed)], signed
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	this->setReg(op.instruction.I.rt, this->memory->getByte(s + imm));
}

void CPU::OpLbu(Operand op)
{
	// R[rt] = 2 bytes from Memory[R[rs] + C(signed)], unsigned
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	this->setReg(op.instruction.I.rt, SignExtImm(this->memory->getByte(s + imm)));
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

void CPU::OpJ(Operand op)
{
}

void CPU::OpJal(Operand op)
{
}

void CPU::OpSra(Operand)
{

}
void CPU::OpJr(Operand)
{

}