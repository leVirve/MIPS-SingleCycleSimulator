/*
 * Define the enviromental parameters
 * 
 * iimage.bin : LE (little endian) encoded.
 *				First 4 bytes for initial PC : the starting address to load the instruction image,
 *				Next 4 bytes specifies the number of words to be loaded into instruction memory (I Memory)
 * dimage.bin : LE (little endian) encoded.
 *				First 4 bytes for initial $sp,
 *				Next 4 bytes indicate the number of words to be loaded into data memory (D Memory)
 */

#include <cstdio>
#include <cstring>

#ifndef __ENV_H__
#define __ENV_H__

#define UINT32 unsigned int
#define INT32 int
#define ITER unsigned int
#define BYTE unsigned char
#define MEMORY_SIZE 1024

#define STAGES 5

/* Status Code */
#define SUCCESS 0
#define UNEXPECTED -1

#define ERR_WRITE_REG_ZERO 0x1
#define ERR_NUMBER_OVERFLOW 0x10
#define ERR_MEMMORY_ADDRESS_OVERFLOW 0x100
#define ERR_DATA_MISALIGNED 0x1000

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

/* Input File */

#define IR_BIN "iimage.bin"
#define DATA_BIN "dimage.bin"


/* Output File */

#define SNAPSHOT "snapshot.rpt"
#define ERR_DUMP "error_dump.rpt"

#endif
