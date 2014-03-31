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
		if(ir_sz > MEMORY_SIZE) throw "Memory Exceed";
		for (unsigned int i = ENTRY_POINT, k = 0; k < 4 * ir_sz; ++k, ++i) {
			BYTE tmp;
			fread(&tmp, sizeof(BYTE), 1, ir);
			// printf("[%02d] %02X\n", i, tmp);
			iMemory[i] = tmp;
		}
	} catch(char* e) {
		fprintf(stderr, "%s\n", e); //
	}
	
	try {
		if(d_sz > MEMORY_SIZE) throw "Memory Exceed";
		for (unsigned int i = 0; i < d_sz; ++i) {
			BYTE tmp;
			fread(&tmp, sizeof(BYTE), 1, data);
			// printf("%02X\n", tmp);
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

void Memory::setSP(UINT32 sp)
{
	this->sp = sp;
}

UINT32 Memory::loadInstruction(UINT32 address)
{
	if (address + 3 >= MEMORY_SIZE) throw "Address overflow";
	return iMemory[address] << 24 |
		iMemory[address + 1] << 16 |
		iMemory[address + 2] << 8 |
		iMemory[address + 3];
}

UINT32 Memory::getWord(UINT32 address)
{
	if (address + 3 >= MEMORY_SIZE) throw "Address overflow";
	return dMemory[address] << 24 |
		dMemory[address + 1] << 16 |
		dMemory[address + 2] << 8 |
		dMemory[address + 3];
}

UINT32 Memory::getHalfWord(UINT32 address)
{
	if (address + 1 >= MEMORY_SIZE) throw "Address overflow";
	return dMemory[address] << 8 |
		dMemory[address + 1];
}

UINT32 Memory::getByte(UINT32 address)
{
	if (address >= MEMORY_SIZE) throw "Address overflow";
	return dMemory[address];
}

void Memory::saveWord(UINT32 address, UINT32 word)
{
	if (address >= MEMORY_SIZE) throw "Address overflow";
	//
}

BYTE Memory::loadByte(UINT32 address)
{
	if (address + 3 >= MEMORY_SIZE) throw "Address overflow";
	//
}

void Memory::saveByte(UINT32 address, BYTE byte)
{
	if (address >= MEMORY_SIZE) throw "Address overflow";
	//
}