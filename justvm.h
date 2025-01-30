/* justvm.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include "utils/utils.h"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wformat-truncation="
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic push

#define NoErr 0x00 /* 00 00 */
#define SysHlt 0x01 /* 00 01 */
#define ErrMem 0x02 /* 00 10 */
#define ErrSegv 0x04 /* 01 00 */
#define ErrInstr 0x08 /* 10 00 */

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int64;

/*
    with virtual cpu and virtual memory (ram)
    65kb virtual memory divided into code segment which is read only and stack for data
	16 bit vm - 65536 memory addresses
	ie 65kb of memory
	general purpose registers - ax, bx, cx, dx
	SP, IP
*/
/*

	opcode is machine code version of the assembly instructions
	mov ax, 0x05 // (0x01 OR ax)
				 in this I will take 8 bits
				 the last bit denotes the operation . eg - for mov 0000 0001
				 0000 0001 - this is the opcode
				 0000 0000 0000 0000 - arguments
				 the other 3 bits before the 1 denotes the register to which we want to mov
				 ax - 0000 1001 16 bit
				 	al - 8bit 11
					ah - 8bit 10
				 bx - 0000 1010
				 cx - 0000 1011
				 dx - 0000 1100
				 sp - 0000 1101 etc
				 FLAGS
				 1010
				 -^--
				 ZERO FLAG
				 CARRY FLAG
				 
				 CMP AX, 0x05
				 p1 EQUAL
				 p2 GREATER THAN
				 p3 HIGH
				 p4	LOW

				 the other 4 will store the value to be moved to the register
				 or like 4 bits means 2^4 combinations
				 so 0000 0000 0000 0000 bits // this is the variable length of arguements
				 5 is 0000 0000 0000 0101
*/

#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)
#define $c (char *)
#define $i (int)


#define $ax ->c.r.ax
#define $bx ->c.r.bx
#define $cx ->c.r.cx
#define $dx ->c.r.dx
#define $sp ->c.r.sp
#define $ip ->c.r.ip
#define $flags ->c.r.flags
#define equal(x) (!!((x $flags &  0x08) >> 3))
#define gt(x) (!!((x $flags &  0x04) >> 2))
#define higher(x) (!!((x $flags &  0x02) >> 1))
#define lower(x) (!!(x $flags &  0x01))
#define segfault(x) error((x), ErrSegv)

typedef int16 Reg;
typedef struct r_registers {
    Reg ax, bx, cx, dx, sp, ip, flags;
	// Reg p1, p2, p3, p4
} Registers;

typedef struct s_cpu {
    Registers r;
} CPU;

typedef enum e_opcode {
    nop = 0x01,
	hlt = 0x02,
    mov = 0x08, /* 0x08 - 0x0f */
	ste = 0x10,
	cle = 0x11,
	stg = 0x12,
	clg = 0x13,
	sth = 0x14,
	clh = 0x15,
	stl = 0x16,
	cll = 0x17,
	/*
	reserved1 = 0x18,
	reserved2 = 0x19
	*/
} Opcode;


typedef struct s_instrmap {
    Opcode o;
    int8 s;
} IM;

typedef int16 Args;
typedef struct s_instruction {
    Opcode o;
    Args a[]; // 0 - 2 bytes
} Instruction;

typedef unsigned char Errorcode;
typedef int8 Memory[((int16)(-1))];
typedef int8 Program;

typedef struct s_vm {
    CPU c;
    Memory m; // is divided between code section and data section which can be executed in the stack
	int16 b; // break line defines where one ends and other starts
} VM;

static IM instrmap[] = {
	{ nop, 0x01 },
	{ hlt, 0x01 },
    { mov, 0x03 }, // define mov as a 3 byte instruction, 1st byte defines its a mov instr and also the reg it moves to 
		{ 0x09, 0x03 }, { 0x0a, 0x03 }, { 0x0b, 0x03 }, 
		{ 0x0c, 0x03 }, { 0x0d, 0x03 }, { 0x0e, 0x03 }, { 0x0f, 0x03 },
		{ ste, 0x01 },
		{ stg, 0x01 },
		{ stl, 0x01 },
		{ sth, 0x01 },
		{ cle, 0x01 },
		{ clg, 0x01 },
		{ cll, 0x01 },
		{ clh, 0x01 }
};
#define IMs (sizeof(instrmap)/sizeof(struct s_instrmap))

void __mov(VM *, Opcode, Args, Args); // moves into reg

void error(VM *, Errorcode); // print error and exit
void execinstr(VM *, Program *); // execute single instruction
void execute(VM *); // execute program
	Program *exampleprogram(VM *);
int8 map(Opcode); // maps an instruction to how many bytes it takes
VM * virtualmachine(void); // constructs new virtual machine structure
int main(int, char**);