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
	void setSP(UINT32);

	UINT32 loadWord(UINT32);
	BYTE loadByte(UINT32);
	void saveWord(UINT32, UINT32);
	void saveByte(UINT32, BYTE);
};

#endif
