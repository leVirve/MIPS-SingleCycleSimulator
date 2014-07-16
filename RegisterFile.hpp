#ifndef __REGISTERFILE_HPP__
#define __REGISTERFILE_HPP__

#include "env.h"

#define REGISTER_NUM 32

class RegisterFile {

private:
	
	// Block the Constructor
	RegisterFile() {
		memset(body, 0, sizeof(body));
	}

	UINT32 body[REGISTER_NUM];

public:

	static RegisterFile& getInstance() {
		static RegisterFile instance;
		return instance;
	}
	
	UINT32 getRegister(int index) {
		if (index < 0 || index >= REGISTER_NUM) 
			throw "Register Access Overflow";

		return body[index];
	}

	void setRegister(int index, UINT32 value) {
		if (index < 0 || index >= REGISTER_NUM) throw "Register Access Overflow";
		if (index == 0) throw ERR_WRITE_REG_ZERO;

		body[index] = value;
	}

};

#endif