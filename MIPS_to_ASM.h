#ifndef __MIPS_TO_ASM_H
#define __MIPS_TO_ASM_H

#include <string.h>

/******************************************************************************/
/*R-type                                                                      */
/******************************************************************************/
#define FUNCT_add	0x20
#define FUNCT_sub	0x22
#define FUNCT_and	0x24
#define FUNCT_or	0x25
#define FUNCT_xor	0x26
#define FUNCT_nor	0x27
#define FUNCT_nand	0x28
#define FUNCT_slt	0x2A
#define FUNCT_sll	0x00
#define FUNCT_srl	0x02
#define FUNCT_sra	0x03
#define FUNCT_jr	0x08
/******************************************************************************/
/*I-type                                                                      */
/******************************************************************************/
#define OP_addi	0x08
#define OP_lw	0x23
#define OP_lh	0x21
#define OP_lhu	0x25
#define OP_lb	0x20
#define OP_lbu	0x24
#define OP_sw	0x2B
#define OP_sh	0x29
#define OP_sb	0x28
#define OP_lui	0x0F
#define OP_andi	0x0C
#define OP_ori	0x0D
#define OP_nori	0x0E
#define OP_slti	0x0A
#define OP_beq	0x04
#define OP_bne	0x05

typedef unsigned int u32;
char str[100];

char* translate_mips_to_asm(u32 instr){
	memset(str, 0, sizeof(str) );

	u32 op = ( (instr>>26) & 0x3F );
	if( op==0x3F ){
		/*S-type*/
		sprintf(str, "halt");
	}else if( op==0x00 ){
		if (instr == 0x0) return "NOP";
		/*R-type*/
		u32 rs = ( (instr >> 21) & 0x1F );
		u32 rt = ( (instr >> 16) & 0x1F );
		u32 rd = ( (instr >> 11) & 0x1F );
		u32 shamt = ( (instr >> 6) & 0x1F );
		u32 funct = ( instr & 0x3F );

		if(funct == FUNCT_add){
			sprintf(str, "add  $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_sub){
			sprintf(str, "sub  $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_and){
			sprintf(str, "and  $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_or){
			sprintf(str, "or   $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_xor){
			sprintf(str, "xor  $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_nor){
			sprintf(str, "nor  $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_nand){
			sprintf(str, "nand $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_slt){
			sprintf(str, "slt  $%d,$%d,$%d", rd, rs, rt);
		}else if(funct == FUNCT_sll){
			sprintf(str, "sll  $%d,$%d,$%d", rd, rt, shamt);
		}else if(funct == FUNCT_srl){
			sprintf(str, "srl  $%d,$%d,$%d", rd, rt, shamt);
		}else if(funct == FUNCT_sra){
			sprintf(str, "sra  $%d,$%d,$%d", rd, rt, shamt);
		}else if(funct == FUNCT_jr){
			sprintf(str, "jr  $%d", rs);
		}else{
			sprintf(str, "Undefine funct!\n");
		}
	}else if( op==0x02 || op==0x03 ){
		/*J-type*/
		u32 address = ( instr & 0x03FFFFFF );
		if(op == 0x02){
			sprintf(str,"j    %08X", address );
		}else{
			sprintf(str,"jal  %08X", address );
		}
	}else{
		/*I-type*/
		u32 rs = ( (instr>>21) & 0x1F );
		u32 rt = ( (instr>>16) & 0x1F );
		u32 immediate = ( instr & 0xFFFF );

		if( op==OP_addi ){
			sprintf(str, "addi $%d,$%d,%d", rt, rs, immediate);
		}else if( op==OP_lw ){
			sprintf(str, "lw   $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_lh ){
			sprintf(str, "lh   $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_lhu ){
			sprintf(str, "lhu  $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_lb ){
			sprintf(str, "lb   $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_lbu ){
			sprintf(str, "lbu  $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_sw ){
			sprintf(str, "sw   $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_sh ){
			sprintf(str, "sh   $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_sb ){
			sprintf(str, "sb   $%d,%d($%d)", rt, immediate, rs);
		}else if( op==OP_lui ){
			sprintf(str, "lui  $%d,%d", rt, immediate);
		}else if( op==OP_andi ){
			sprintf(str, "andi $%d,$%d,%d", rt, rs, immediate);
		}else if( op==OP_ori ){
			sprintf(str, "ori  $%d,$%d,%d", rt, rs, immediate);
		}else if( op==OP_nori ){
			sprintf(str, "nori $%d,$%d,%d", rt, rs, immediate);
		}else if( op==OP_slti ){
			sprintf(str, "slti $%d,$%d,%d", rt, rs, immediate);
		}else if( op==OP_beq ){
			sprintf(str, "beq  $%d,$%d,%d", rs, rt, immediate);
		}else if( op==OP_bne ){
			sprintf(str, "bne  $%d,$%d,%d", rs, rt, immediate);
		}else{
			sprintf(str, "Undefine opcode!");
		}
	}

	return &str[0];
}

#endif
