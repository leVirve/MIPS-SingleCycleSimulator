/*
 *
 * MIPS Single Cycle Simulator
 *
 * Develop: TaiwanGoodGood
 * Maintainance Member: Salas , Venxarmy
 *
 * Last Time Update: 2014/04
 *
 */

#include "env.h"
#include "CPU.h"
#include "Memory.h"
#include "Reporter.h"
#include "errDump.h"

CPU* vcpu;
Memory* memory;

Reporter reporter;
ErrDumper errdumper;

UINT32 ENTRY_POINT;
UINT32 STACK_POINT;

void loader()
{
	UINT32 &pc = ENTRY_POINT;
	UINT32 &sp = STACK_POINT;
	FILE* ir = fopen("iimage.bin", "rb");
	FILE* data = fopen("dimage.bin", "rb");
	try {
		if (ir == 0 || data == 0) throw "No File";
		fread(&pc, sizeof(UINT32), 1, ir);
		fread(&sp, sizeof(UINT32), 1, data);
	} catch (const char* e) {
		fprintf(stderr, "%s\n", e); //
		throw "Loader Err";
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

	try {
		loader();
	} catch(const char* e) {
		puts(e);
		return 1;	
	}	
	unsigned int cycle = 0;
	bool _halt = false;
	while (1) {

		printf("PC: %X\nSP: %X\n", ENTRY_POINT, STACK_POINT); //
		printf("-- Round %d \n", cycle + 1);//
// if(cycle  == 26) break;
		try {
			reporter.write(vcpu, cycle);
			++cycle;
			UINT32 pc = vcpu->PC();
			UINT32 instr = vcpu->fetch(memory->loadInstruction(pc));
			Operand operand = vcpu->decode(instr);
			_halt = vcpu->exec(operand);
		}
		catch (UINT32 expCode) {
			char errInfo[100] = {};
			if (expCode & ERR_WRITE_REG_ZERO) {
				sprintf(errInfo, "Write $0 error in cycle: %d", cycle);
				errdumper.write(errInfo);
			}
			if (expCode & ERR_NUMBER_OVERFLOW) {
				sprintf(errInfo, "Number overflow in cycle: %d", cycle);
				errdumper.write(errInfo);
			}
			if (expCode & ERR_MEMMORY_ADDRESS_OVERFLOW) {
				sprintf(errInfo, "Address overflow in cycle: %d", cycle);
				errdumper.write(errInfo);
				_halt = true;
			}
			if (expCode & ERR_DATA_MISALIGNED) {
				sprintf(errInfo, "Misalignment error in cycle: %d", cycle);
				errdumper.write(errInfo);
				_halt = true;
			}

			printf("EXP: %X !!!!!!!!!!!!!!\n", expCode);

		}
		if (_halt == true) break;
	}
#ifdef _DEBUG
		puts("\n---- Simulatation Stops ! -----\n");//
#endif
	return 0;
}
