#include "Memory.h"

Memory::Memory()
{
	memset(iMemory, 0, MEMORY_SIZE);
	memset(dMemory, 0, MEMORY_SIZE);
}

void Memory::load(FILE* ir, FILE* data, UINT32 ENTRY_POINT)
{
	UINT32 ir_sz, d_sz;

	fread(&ir_sz, sizeof(UINT32), 1, ir);
	fread(&d_sz, sizeof(UINT32), 1, data);

	try {
#ifdef _DEBUG
		///if(ir_sz > MEMORY_SIZE) throw "Memory Exceed";
#endif
		for (unsigned int k = 0; k < 4 * ir_sz; ++k) {
			fread(&iMemory[ENTRY_POINT + k], sizeof(BYTE), 1, ir);
		}
	} catch(char* e) {
		fprintf(stderr, "%s\n", e); //
	}
	fclose(ir);
int k = 1;////
	try {
		///if(d_sz > MEMORY_SIZE) throw "Memory Exceed";
		for (unsigned int i = 0; i < 4 * d_sz; ++i) {
			int ss= fread(&dMemory[i], sizeof(BYTE), 1, data);
			if (ss) k++;
		}
printf("read succ : 0x%X bytes\n", k);
	} catch(char* e) {
		fprintf(stderr, "%s\n", e); //
	}
	fclose(data);

#ifdef _DEBUG
	printf("IR Size: %d\n", ir_sz);
	printf("Data Size: %d\n", d_sz);
#endif
}

UINT32 Memory::loadInstruction(UINT32 address)
{
	if (address >= MEMORY_SIZE) throw (UINT32)ERR_MEMMORY_ADDRESS_OVERFLOW;
	if (address % 4 != 0) throw (UINT32) ERR_DATA_MISALIGNED;
	// only combine the information (no endian), no need to sort
	///if (address + 3 >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	return iMemory[address] << 24 |
		iMemory[address + 1] << 16 |
		iMemory[address + 2] << 8 |
		iMemory[address + 3];
}

UINT32 Memory::getWord(UINT32 address)
{
	///if (address + 3 >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	return dMemory[address] |
		dMemory[address + 1] << 8 |
		dMemory[address + 2] << 16 |
		dMemory[address + 3] << 24;
}

UINT32 Memory::getHalfWord(UINT32 address)
{
	///if (address + 1 >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	return dMemory[address] |
		dMemory[address + 1] << 8;
}

UINT32 Memory::getByte(UINT32 address)
{
	///if (address >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	return dMemory[address];
}

void Memory::saveWord(UINT32 address, UINT32 word)
{
	///if (address + 3 >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	dMemory[address] = (word >> 0) & 0xff;
	dMemory[address + 1] = (word >> 8) & 0xff;
	dMemory[address + 2] = (word >> 16) & 0xff;
	dMemory[address + 3] = (word >> 24) & 0xff;
}

void Memory::saveHalfWord(UINT32 address, UINT32 halfword)
{
	///if (address + 1 >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	dMemory[address] = (halfword >> 0) & 0xff;
	dMemory[address + 1] = (halfword >> 8) & 0xff;
}

void Memory::saveByte(UINT32 address, UINT32 byte)
{
	///if (address >= MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
	dMemory[address] = byte;
}