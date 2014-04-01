#ifndef __MEMORY__H__
#define __MEMORY__H__

#include "env.h"

class Memory {

private:
	BYTE iMemory[MEMORY_SIZE];
	BYTE dMemory[MEMORY_SIZE];
	UINT32 sp;
public:
	Memory();
	void load(FILE*, FILE*, UINT32);

	UINT32 loadInstruction(UINT32);
	UINT32 getWord(UINT32);
	UINT32 getHalfWord(UINT32);
	UINT32 getByte(UINT32);
	void saveWord(UINT32, UINT32);
	void saveHalfWord(UINT32, UINT32);
	void saveByte(UINT32, UINT32);
#ifdef _DEBUG
	void printMemory();
#endif
};

#endif
