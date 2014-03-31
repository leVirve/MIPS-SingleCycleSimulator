#include "env.h"
#include "CPU.h"
#include "Memory.h"
#include <cstdio>

void loader(char* IR, char* DATA)
{
	FILE* ir = fopen(IR, "r");
	FILE* data = fopen(DATA, "r");

	UINT32 ipc, ir_sz;
	UINT32 sp, d_sz;

	fread(&ipc, sizeof(UINT32), 1, ir);
	fread(&ir_sz, sizeof(UINT32), 1, ir);
	fread(&sp, sizeof(UINT32), 1, data);
	fread(&d_sz, sizeof(UINT32), 1, data);

	printf("Inital PC: %d\nIR Size: %d\n", ipc, ir_sz);
	for (int i = 0; i < ir_sz; ++i)
	{
		UINT32 rr;
		fread(&rr, sizeof(UINT32), 1, ir);
		printf("%08X\n", rr);
	}
	puts("");
	printf("Inital SP: %d\nData Size: %d\n", sp, d_sz);
	for (int i = 0; i < d_sz; ++i)
	{
		UINT32 rr;
		fread(&rr, sizeof(UINT32), 1, data);
		printf("%08X\n", rr);
	}
}

int main()
{
	
	loader((char*)IR_image, (char*)DATA_image);
	while(1);
	return 0;
}
