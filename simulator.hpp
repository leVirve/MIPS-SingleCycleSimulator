#include "env.h"
#include "Memory.hpp"
#include "RegisterFile.hpp"
#include "Register.h"
#include "Stage.hpp"

RegisterMap registers;

class Simulator {

private:

	Memory& iMemory;
	Memory& dMemory;
	RegisterFile& reg;

	//Reporter reporter;
	//ErrDumper errdumper;

	UINT32 ENTRY_POINT;
	UINT32 STACK_POINT;
	UINT32 cycle;
	bool halt;

public:

	Simulator():
		iMemory (Memory::getInstructionMemory()),
		dMemory (Memory::getDataMemory()),
		reg     (RegisterFile::getInstance())
		{
		cycle = 0;
		halt = false;
	}

	int loader() {
		FILE* instruction = fopen("iimage.bin", "rb");
		FILE* data = fopen("dimage.bin", "rb");

		try {
			if (instruction == NULL) throw "No instrction image";
			if (data == NULL)        throw "No data image";
			fread(&ENTRY_POINT, sizeof(UINT32), 1, instruction);
			fread(&STACK_POINT, sizeof(UINT32), 1, data);

			UINT32 instruction_size, data_size;
			BYTE tmp;

			// Set the infomation
			registers["PC"] = ENTRY_POINT;
			reg.setRegister(29, STACK_POINT);

			// Read in the limitation
			fread(&instruction_size, sizeof(UINT32), 1, instruction);
			fread(&data_size, sizeof(UINT32), 1, data);
			if (instruction_size >= MEMORY_SIZE) throw "Instruction Size Exceed Memory Size";
			if (data_size >= MEMORY_SIZE)        throw "Data Size Exceed Memory Size";

			// Read into Memory
			for (ITER k = 0; k < 4 * instruction_size; ++k)
				fread(&tmp, sizeof(BYTE), 1, instruction),
				iMemory.loadInto(k + ENTRY_POINT, tmp);

			for (ITER k = 0; k < 4 * data_size; ++k)
				fread(&tmp, sizeof(BYTE), 1, data),
				dMemory.loadInto(k, tmp);

		}
		catch (const char* e) {
			fprintf(stderr, "%s\n", e);
			return UNEXPECTED;
		}

		return SUCCESS;
	}

	int start() {

		if (this->loader() != SUCCESS) return UNEXPECTED;

		printf("PC: %X\nSP: %X\n", ENTRY_POINT, STACK_POINT);

		Stage* stages[STAGES] = {
			new WriteBackStage(),
			new MemoryAccessStage(),
			new ExecuteStage(),
			new DecodeStage(),
			new FetchStage()
		};

		
		try {
			while (true) {
				//reporter.write(vcpu, cycle);
				if (cycle == 23) return UNEXPECTED;

				printf("\nCycle %d: \n", ++cycle);
				for (ITER i = 0; i < STAGES; ++i) {
					
					halt = stages[i]->execute();
					stages[i]->toString();

					if (halt) break;
				}
				if (halt) break;
			}
		} catch (UINT32 expCode) {
				printf("Ex %d\n", expCode);
		}

		return SUCCESS;
	}

};
