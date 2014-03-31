#include "env.h"
#include "CPU.h"

class Reporter {
public:
	Reporter();
	~Reporter();
	void write(CPU*, unsigned int);
private:
	FILE* reg_snapshot;
};

Reporter::Reporter()
{
	reg_snapshot = fopen(SNAPSHOT, "w");
}

Reporter ::~Reporter()
{
}

void Reporter::write(CPU* vcpu, unsigned int cycle)
{
	fprintf(reg_snapshot, "cycle %d\n", cycle);
	for (int i = 0; i < 32; ++i)
		fprintf(reg_snapshot, "$%02d: 0x%08X\n", i, vcpu->getReg(i));
	fprintf(reg_snapshot, "PC: 0x%08X\n", vcpu->getPC());
	fprintf(reg_snapshot, "\n\n");
}