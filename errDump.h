#ifndef __ERR_DUMP__
#define __ERR_DUMP__

#include "env.h"

class ErrDumper {

private:

	FILE* dumpfile;

public :

	ErrDumper();
	~ErrDumper();
	void write(char*);

};

ErrDumper::ErrDumper()
{
	dumpfile = fopen(ERR_DUMP, "w");
}

ErrDumper::~ErrDumper()
{
	fclose(dumpfile);
}

void ErrDumper::write(char* err_info)
{
	fprintf(dumpfile, "%s\n", err_info);
}

#endif