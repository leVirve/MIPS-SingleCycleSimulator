#ifndef __STRUCT_H__
#define __STRUCT_H__

#define R_TYPE 'R'
#define I_TYPE 'I'
#define J_TYPE 'J'
#define HALT 'H'

struct R_type {
	unsigned func : 6;
	unsigned shamt : 5;
	unsigned rd : 5;
	unsigned rt : 5;
	unsigned rs : 5;
	unsigned opcode : 6;
};

struct I_type {
	unsigned immediate : 16;
	unsigned rt : 5;
	unsigned rs : 5;
	unsigned opcode : 6;
};

struct J_type {
	unsigned address : 26;
	unsigned opcode : 6;
};

struct Terminate {
	unsigned opcode : 6;
	unsigned : 26;
};

union IR_Type {
	R_type R;
	I_type I;
	J_type J;
};

struct  Operand {
	char tpye;
	IR_Type instruction;
};

#endif