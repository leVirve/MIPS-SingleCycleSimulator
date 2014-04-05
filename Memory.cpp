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
			BYTE tmp;
			fread(&tmp, sizeof(BYTE), 1, ir);
			iMemory[ENTRY_POINT + k] = tmp;
		}
	} catch(char* e) {
		fprintf(stderr, "%s\n", e); //
	}
	
	try {
		///if(d_sz > MEMORY_SIZE) throw "Memory Exceed";
		for (unsigned int i = 0; i < 4 * d_sz; ++i) {
			BYTE tmp;
			fread(&tmp, sizeof(BYTE), 1, data);
			dMemory[i] = tmp;
		}
	} catch(char* e) {
		fprintf(stderr, "%s\n", e); //
	}

#ifdef _DEBUG
	printf("IR Size: %d\n", ir_sz);
	printf("Data Size: %d\n", d_sz);
#endif
}

UINT32 Memory::loadInstruction(UINT32 address)
{
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

#ifdef _DEBUG

void Memory::printMemory()
{
	for (int i = 0; i < 12; ++i)
		printf("%X ", dMemory[i]);
}

#endif