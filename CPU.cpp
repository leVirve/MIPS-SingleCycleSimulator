#include "CPU.h"

/*
 * Initialization of CPU object
 * - clean up the $reg[] to 0
 * - setup PC
 * - set Memory known by CPU
 * - set(store) SP into reg[$sp]
 */

CPU::CPU()
{
	memset(reg, 0, sizeof(reg));
}

void CPU::setMemory(Memory* _memory)
{
	this->memory = _memory;
}

void CPU::setPC(UINT32 pc)
{
	this->pc = pc;
}

void CPU::setSP(UINT32 sp)
{
	this->setReg(29, sp);
}

UINT32 CPU::getPC()
{
	return pc;
}

/**
 * PC increment
 * @return before increment PC
 */
UINT32 CPU::PC()
{
	UINT32 current_pc = pc;
	pc = pc + 4;
	return current_pc;
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

	printf("Little Endian IR: %08x\n", _instruction);
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
	UINT32 _type = (instruction >> 26) & 0x3f;

	printf("OpCode: %02X\n", _type);

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
	// try-catch the following errors:
	// . Write Reg[0] error
	// . Number overflow
	// . I-Memory Address Overflow
	// . D-Memory Address Overflow
	// . Misalign Error
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
	return false;
}

UINT32 CPU::getReg(int idx)
{
	return reg[idx];
}

void CPU::setReg(int targ, UINT32 content)
{
	// ERR_Detection: check for REG$0
	if (targ == 0) return; // Do not write the data in.
		/* no use in this project: 
			 throw (UINT32)ERR_WRITE_REG_ZERO; */
	reg[targ] = content;
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
		case 0x08:
			OpAddi(instr);
			break;
		case 0x23:
			OpLw(instr);
			break;
		case 0x21:
			OpLh(instr);
			break;
		case 0x25:
			OpLhu(instr);
			break;
		case 0x20:
			OpLb(instr);
			break;
		case 0x24:
			OpLbu(instr);
			break;
		case 0x2B:
			OpSw(instr);
			break;
		case 0x29:
			OpSh(instr);
			break;
		case 0x28:
			OpSb(instr);
			break;
		case 0x0F:
			OpLui(instr);
			break;
		case 0x0C:
			OpAndi(instr);
			break;
		case 0x0D:
			OpOri(instr);
			break;
		case 0x0E:
			OpNori(instr);
			break;
		case 0x0A:
			OpSlti(instr);
			break;
		case 0x04:
			OpBeq(instr);
			break;
		case 0x05:
			OpBne(instr);
			break;
	}
}

void CPU::J_exec(Operand instr)
{
	switch (instr.instruction.J.opcode) {
		case 0x02:
			OpJ(instr);
			break;
		case 0x03:
			OpJal(instr);
			break;
	}
}

/*
* @param  (16-bit immediate field) t
* @return (32-bit) signed_ext
*/
UINT32 CPU::SignExtImm(UINT32 t)
{
	// {16{immediate[15]}, immediate}
	if ((t >> 15) == 0x0)
		return t & 0x0000ffff;
	else
		return t | 0xffff0000;
}

/*
* @param  (8-bit immediate field) t
* @return (32-bit) signed_ext
*/
UINT32 CPU::SignExtImmb(UINT32 t)
{
	// {16{immediate[15]}, immediate}
	if ((t >> 7) == 0x0)
		return t & 0x000000ff;
	else
		return t | 0xffffff00;
}

/*
 * @param  (16-bit immediate field) t
 * @return (32-bit) zero_ext
 */
UINT32 CPU::ZeroExtImm(UINT32 t)
{
	// 16{1b'0}, immediate
	return t & 0x0000ffff;
}

/*
 * @param (16-bit immediate field) t
 * @return (32-bit) address
 */
UINT32 CPU::BranchAdrr(UINT32 t)
{
	// BranchAddr = {14{immediate[15]}, immediate, 2'b0}
	if((t >> 15) == 0x0)
		return 0x0003ffff & (t << 2);
	else 
		return 0xfffc0000 | (t << 2);
}

/*
 * @param (26-bit immediate field) t
 * @return (32-bit) address
 */
UINT32 CPU::JumpAddr(UINT32 t)
{
	// JumpAddr = {PC[31:28], address, 2'b0}
	/* already +4 after fetch */
	UINT32 addr = this->getPC() & 0xf0000000;
	return addr | (t << 2);
}

/*
 * @param addends :a, b
 * @return isOverflow ? true : false
 */
bool isOverflow(UINT32 a, UINT32 b)
{
	UINT32 sum = a + b;
	// if signs of a, b are the same 
	if (((a >> 31) ^ (b >> 31)) == 0)
		if (((a >> 31) ^ (sum >> 31)) == 1) return true;
	return false;
}

/*
* @param  (32-bit) t
* @return (32-bit) ~t + 1
*/
UINT32 twoComplement(UINT32 t)
{
		/* no need in this project
			if (t == 0x80000000) 
				throw ERR_NUMBER_OVERFLOW; */
	return ~ t + 1;
}

/* R - Type */
void CPU::OpAdd(Operand op)
{
	// R[rd] = R[rs] + R[rt]
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and continue this instruction
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 val = s + t;
#ifdef _DEBUG
	printf("%X(%d) + %X(%d) = %X(%d)\n", s, s, t, t, val, val);
#endif
	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, t)) err |= ERR_NUMBER_OVERFLOW; // continue
	this->setReg(op.instruction.R.rd, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpSub(Operand op)
{
	// R[rd] = R[rs] - R[rt]
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow | t = -2^31 message, and continue this instruction 
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 new_t = twoComplement(t);
	UINT32 val = s + new_t;
printf("Sub %d %d\n", s, t);
	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, new_t) || t == 0x80000000) err |= ERR_NUMBER_OVERFLOW; // continue
	this->setReg(op.instruction.R.rd, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpAnd(Operand op)
{
	// R[rd] = R[rs] & R[rt]
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, s & t);
	if (err != 0) throw err; // send error message
}
void CPU::OpOr(Operand op)
{
	// R[rd] = R[rs] | R[rt]
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, s | t);
	if (err != 0) throw err; // send error message
}
void CPU::OpXor(Operand op)
{
	// R[rd] = R[rs] ^ R[rt]
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, s ^ t);
	if (err != 0) throw err; // send error message
}
void CPU::OpNor(Operand op)
{
	// R[rd] = ~(R[rs] | R[rt])
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, ~(s | t));
	if (err != 0) throw err; // send error message
}
void CPU::OpNand(Operand op)
{
	// R[rd] = ~(R[rs] & R[rt])
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.R.rs);
	UINT32 t = this->getReg(op.instruction.R.rt);

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, ~(s & t));
	if (err != 0) throw err; // send error message
}
void CPU::OpSlt(Operand op)
{
	// R[rd] = R[rs] < R[rt] ? 1 : 0
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	/* Use Signed Number Compare ! */
	INT32 s = this->getReg(op.instruction.R.rs);
	INT32 t = this->getReg(op.instruction.R.rt);

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, s < t ? 1 : 0);
	if (err != 0) throw err; // send error message
}

void CPU::OpSll(Operand op)
{
	// R[rd] = R[rt] << shamt
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 shamt = op.instruction.R.shamt;

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, t << shamt);
	if (err != 0) throw err; // send error message
}

void CPU::OpSrl(Operand op)
{
	// R[rd] = R[rt] >> shamt
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 t = this->getReg(op.instruction.R.rt);
	UINT32 shamt = op.instruction.R.shamt;

	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, t >> shamt);
	if (err != 0) throw err; // send error message
}

void CPU::OpSra(Operand op)
{
	// R[rd] = R[rt] >> shamt, with signed bit shifted in (arithmetically)
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	INT32  t = this->getReg(op.instruction.R.rt);
	UINT32 shamt = op.instruction.R.shamt;
	UINT32 val = (t >= 0) ? t >> shamt
						: (t >> shamt) | ~( 0xffffffff >> shamt );
printf("SRA %d(%d) >> %d ->%d(%d)\n", t, op.instruction.R.rt, shamt, val, op.instruction.R.rd);
	if (op.instruction.R.rd == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.R.rd, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpJr(Operand op)
{
	// PC = R[rs]
	UINT32 s = this->getReg(op.instruction.R.rs);
	this->setPC(s);
}

/* I-type */
void CPU::OpAddi(Operand op)
{
	// R[rt] = R[rs] + immediate(signed)	
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and execute this instruction
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	UINT32 val = s + imm;
printf("%d %d\n",s, imm );
	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW; // continue
	this->setReg(op.instruction.I.rt, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpLw(Operand op)
{
	// R[rt] = 4 bytes from Memory[R[rs] + C(signed)]
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and continue this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	// ERR_Detection : throw misalignment message, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
printf("LW %d %d\n", s, imm);
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW; // halt
	if ((s + imm) % 4 != 0) err |= ERR_DATA_MISALIGNED; // halt
	if((err & ERR_MEMMORY_ADDRESS_OVERFLOW) || (err & ERR_DATA_MISALIGNED)) { printf("WTF %d\n", err); throw err;}
	this->setReg(op.instruction.I.rt, this->memory->getWord(s + imm));
printf("__ data= %X(%d)\n", this->getReg(op.instruction.I.rt), op.instruction.I.rt);
	if (err != 0) throw err; // send error message
}

void CPU::OpLh(Operand op)
{
	// R[rt] = 2 bytes from Memory[R[rs] + C(signed)], signed
	// Use SignExt()
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and continue this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	// ERR_Detection : throw misalignment message, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW; // halt
	if ((s + imm) % 2 != 0) err |= ERR_DATA_MISALIGNED; // halt
	if((err & ERR_MEMMORY_ADDRESS_OVERFLOW) || (err & ERR_DATA_MISALIGNED)) throw err;
	UINT32 val = SignExtImm(this->memory->getHalfWord(s + imm));
	this->setReg(op.instruction.I.rt, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpLhu(Operand op)
{
	// R[rt] = 2 bytes from Memory[R[rs] + C(signed)], unsigned
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and continue this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	// ERR_Detection : throw misalignment message, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW; // halt
	if ((s + imm) % 2 != 0) err |= ERR_DATA_MISALIGNED; // halt
	if((err & ERR_MEMMORY_ADDRESS_OVERFLOW) || (err & ERR_DATA_MISALIGNED)) throw err;
	UINT32 val = this->memory->getHalfWord(s + imm);
	this->setReg(op.instruction.I.rt, val & 0xffff);
	if (err != 0) throw err; // send error message
}

void CPU::OpLb(Operand op)
{
	// R[rt] = 1 bytes from Memory[R[rs] + C(signed)], signed
	// Use SignExtb()  SignExt for val must use 8-bit extention
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and execute this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW, throw err; // halt
	UINT32 val = SignExtImmb(this->memory->getByte(s + imm));
	this->setReg(op.instruction.I.rt, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpLbu(Operand op)
{
	// R[rt] = 1 bytes from Memory[R[rs] + C(signed)], unsigned
	// ERR_Detection : throw write reg $0
	// ERR_Detection : throw overflow message, and execute this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW, throw err; // halt
	UINT32 val = this->memory->getByte(s + imm);
	this->setReg(op.instruction.I.rt, val & 0xff);
	if (err != 0) throw err; // send error message
}

void CPU::OpSw(Operand op)
{
	// 4 bytes in Memory[R[rs] + C(signed)] = R[rt] 
	// ERR_Detection : throw overflow message, and continue this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	// ERR_Detection : throw misalignment message, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	UINT32 val = this->getReg(op.instruction.I.rt);

	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW; // halt
	if ((s + imm) % 4 != 0) err |= ERR_DATA_MISALIGNED; // halt
	if((err & ERR_MEMMORY_ADDRESS_OVERFLOW) || (err & ERR_DATA_MISALIGNED)) throw err;
	this->memory->saveWord(s + imm, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpSh(Operand op)
{
	// 2 bytes in Memory[R[rs] + C(signed)], signed = R[rt]
	// ERR_Detection : throw overflow message, and execute this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	// ERR_Detection : throw misalignment message, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	UINT32 val = this->getReg(op.instruction.I.rt) & 0xffff;

	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW; // halt
	if ((s + imm) % 2 != 0) err |= ERR_DATA_MISALIGNED; // halt
	if((err & ERR_MEMMORY_ADDRESS_OVERFLOW) || (err & ERR_DATA_MISALIGNED)) throw err;
	this->memory->saveHalfWord(s + imm, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpSb(Operand op)
{
	// 2 bytes in Memory[R[rs] + C(signed)], signed = R[rt]
	// ERR_Detection : throw overflow message, and execute this instruction
	// ERR_Detection : throw memory address overflow, and halt simulation
	// ERR_Detection : throw misalignment message, and halt simulation
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = SignExtImm(op.instruction.I.immediate);
	UINT32 val = this->getReg(op.instruction.I.rt) & 0xff;

	if (isOverflow(s, imm)) err |= ERR_NUMBER_OVERFLOW;  // continue
	if ((s + imm) >= MEMORY_SIZE) err |= ERR_MEMMORY_ADDRESS_OVERFLOW, throw err; // halt
	this->memory->saveByte(s + imm, val);
	if (err != 0) throw err; // send error message
}

void CPU::OpLui(Operand op)
{
	// R[rt] = immediate << 16
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 imm = op.instruction.I.immediate;
	imm = imm << 16;

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.I.rt, imm);
	if (err != 0) throw err; // send error message
}

void CPU::OpAndi(Operand op)
{
	// R[rt] = R[rs] & immediate(unsigned)
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = ZeroExtImm(op.instruction.I.immediate);
	s &= imm;

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.I.rt, s);
	if (err != 0) throw err; // send error message
}
void CPU::OpOri(Operand op)
{
	// R[rt] = R[rs] | immediate(unsigned)
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = ZeroExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	s |= imm;
	this->setReg(op.instruction.I.rt, s);
	if (err != 0) throw err; // send error message
}
void CPU::OpNori(Operand op)
{
	// R[rt] = ~( R[rs] | immediate(unsigned) )
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 imm = ZeroExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	s = ~(s | imm);
	this->setReg(op.instruction.I.rt, s);
	if (err != 0) throw err; // send error message
}
void CPU::OpSlti(Operand op)
{
	// R[rt] = (R[rs] < immediate(signed)) ? 1 : 0, signed comparison
	// ERR_Detection : throw write reg $0
	UINT32 err = 0;
	/* Use Signed Number Compare ! */
	INT32 s = this->getReg(op.instruction.I.rs);
	INT32 imm = SignExtImm(op.instruction.I.immediate);

	if (op.instruction.R.rt == 0) err |= ERR_WRITE_REG_ZERO; // continue
	this->setReg(op.instruction.I.rt, s < imm ? 1 : 0);
	if (err != 0) throw err; // send error message
}
void CPU::OpBeq(Operand op)
{
	// if (R[rs] == R[rt]) PC = PC + BranchAddr
	/* already +4 after fetch */
	/* Signed Operation on imm */
	// ERR_Detection : throw overflow message, and execute this instruction
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 t = this->getReg(op.instruction.I.rt);
	INT32 imm = BranchAdrr(op.instruction.I.immediate);
#ifdef _DEBUG
	printf("%X(%d) %X(%d) = %X(%d)\n", this->pc, this->pc, imm, imm, this->pc + imm, this->pc + imm);
#endif
	if (isOverflow(this->pc, imm)) err |= ERR_NUMBER_OVERFLOW;
	if (s == t) this->setPC(this->pc + imm);
	if (err != 0) throw err; // send error message
}
void CPU::OpBne(Operand op)
{
	// if (R[rs] == R[rt]) PC = PC + BranchAddr
	/* already +4 after fetch */
	/* Signed Operation on imm */
	// ERR_Detection : throw overflow message, and execute this instruction
	UINT32 err = 0;
	UINT32 s = this->getReg(op.instruction.I.rs);
	UINT32 t = this->getReg(op.instruction.I.rt);
	INT32 imm = BranchAdrr(op.instruction.I.immediate);

	if (isOverflow(this->pc, imm)) err |= ERR_NUMBER_OVERFLOW;
	if (s != t) this->setPC(this->pc + imm);
	if (err != 0) throw err; // send error message
}

/* J-type */
void CPU::OpJ(Operand op)
{
	// PC = JumpAddr
	this->setPC(JumpAddr(op.instruction.J.address));
}

void CPU::OpJal(Operand op)
{
	// R[31] = PC
	// PC = JumpAddr
	/* already +4 after fetch */
	this->setReg(31, this->getPC());
	this->setPC(JumpAddr(op.instruction.J.address));
}
