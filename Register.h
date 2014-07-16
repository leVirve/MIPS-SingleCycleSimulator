#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <map>
#include <string>
using namespace std;

typedef map<string, unsigned int> RegisterMap;
typedef map<unsigned int, unsigned int> ErrorRegister;
// body in simulator.hpp
extern RegisterMap registers;
extern ErrorRegister err_registers;

#endif
