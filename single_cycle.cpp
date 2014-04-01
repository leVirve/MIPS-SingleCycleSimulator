/*
 *
 * MIPS Single Cycle Simulator
 *
 * Contains:
 * - loader
 * - Reporter
 * - CPU
 * - Memory
 */

#include "env.h"
#include "Reporter.h"
#include "CPU.h"
#include "Memory.h"

CPU* vcpu;
Memory* memory;

Reporter reporter;

UINT32 ENTRY_POINT;
UINT32 STACK_POINT;

void loader()
{
	UINT32 &pc = ENTRY_POINT;
	UINT32 &sp = STACK_POINT;
	FILE* ir = fopen(IR_BIN, "r");
	FILE* data = fopen(DATA_BIN, "r");
	try {
		if (ir == NULL || data == NULL) throw "No File";
		fread(&pc, sizeof(UINT32), 1, ir);
		fread(&sp, sizeof(UINT32), 1, data);
	} catch (char* e) {
		fprintf(stderr, "%s\n", e); //
	}

	memory->load(ir, data, ENTRY_POINT);
	vcpu->setPC(ENTRY_POINT);
	vcpu->setSP(STACK_POINT);
	vcpu->setMemory(memory);
}

int main()
{
	vcpu = new CPU();
	memory = new Memory();
	
	loader();
#ifdef _DEBUG
	printf("PC: %X\nSP: %X\n", ENTRY_POINT, STACK_POINT); //
#endif	
	unsigned int cycle = 0;
	bool _halt = false;
	while(1) {
#ifdef _DEBUG	
		printf("-- Round %d \n", cycle);//
#endif
		reporter.write(vcpu, cycle++);
		UINT32 pc = vcpu->PC();
		UINT32 instr = vcpu->fetch(memory->loadInstruction(pc));
		Operand operand;
		try {
			operand = vcpu->decode(instr);
		} catch (char* e) {
			// errDump.write();
		}
		memory->printMemory();//
		_halt = vcpu->exec(operand);
		if (_halt == true) break;
	}
#ifdef _DEBUG
	puts("\n---- Simulatation Stops ! -----\n");//
#endif
	return 0;
}
