#ifndef __MEMORY__H__
#define __MEMORY__H__

#include "env.h"



class Memory {

private:

	// Singleton Pattern
	Memory() {
		body = new BYTE[MEMORY_SIZE];
		memset(body, 0, MEMORY_SIZE);
	}
	
	BYTE* body;

public:

	static Memory& getInstructionMemory() {
		static Memory instructionInstance;
		return instructionInstance;
	}

	static Memory& getDataMemory() {
		static Memory dataInstance;
		return dataInstance;
	}

	void loadInto(UINT32 index, BYTE byte) {
		this->body[index] = byte;
	}

	// only concact the data (no endian), no need to sort
	UINT32 getInstruction(UINT32 address) {
		return body[address]  << 24 |
			body[address + 1] << 16 |
			body[address + 2] <<  8 |
			body[address + 3];
	}

	UINT32 getWord(UINT32 address) {
		// if (address + 3 > MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
		return body[address] |
			body[address + 1] << 8  |
			body[address + 2] << 16 |
			body[address + 3] << 24;
	}

	UINT32 getHalfWord(UINT32 address) {
	//if (address + 1 > MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
		return body[address] |
			body[address + 1] << 8;
	}

	UINT32 getByte(UINT32 address) {
		///if (address > MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
		return body[address];
	}

	void saveWord(UINT32 address, UINT32 word) {
		//if (address + 3 > MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
		body[address] = (word >> 0) & 0xff;
		body[address + 1] = (word >> 8) & 0xff;
		body[address + 2] = (word >> 16) & 0xff;
		body[address + 3] = (word >> 24) & 0xff;
	}

	void saveHalfWord(UINT32 address, UINT32 halfword) {
		//if (address + 1 > MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
		body[address] = (halfword >> 0) & 0xff;
		body[address + 1] = (halfword >> 8) & 0xff;
	}

	void Memory::saveByte(UINT32 address, UINT32 byte) {
		//if (address > MEMORY_SIZE) throw ERR_MEMMORY_ADDRESS_OVERFLOW;
		body[address] = byte;
	}


};

#endif
