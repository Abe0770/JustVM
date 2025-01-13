/* justvm.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
	SP - stack pointer
	IP - instruction pointer
	(Serial COM port)
	(Floppy drive)
*/
/*

	opcode is machine code version of the assembly instructions
	mov ax, 0x05 // (0x01 OR ax)
				 in this I will take 8 bits
				 the last bit denotes the operation . eg - for mov 0000 0001
				 0000 0001 - this is the opcode
				 0000 0000 0000 0000 - arguments
				 the other 3 bits before the 1 denotes the register to which we want to mov
				 ax - 0000 0011
				 bx - 0000 0101
				 cx - 0000 0111
				 dx - 0000 1001
				 sp - 0000 1011 etc
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
#define segfault(x) error((x), ErrSegv)

typedef unsigned short int Reg;
typedef struct r_registers {
    Reg ax, bx, cx, dx, sp, ip; // General purpose, stack pointer, instruction pointer
} Registers;

typedef struct s_cpu {
    Registers r;
} CPU;

typedef enum e_opcode {
    mov = 0x01,
    nop = 0x02,
	hlt = 0x03
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
    Memory m; // memory
	int16 b;
} VM;

typedef Memory *Stack;

static IM instrmap[] = {
    { mov, 0x03 },
    { nop, 0x01 },
	{ hlt, 0x01 }
};
#define IMs (sizeof(instrmap) / sizeof(struct s_instrmap))

void __mov(VM *, Opcode, Args, Args);

void error(VM *, Errorcode);
void execinstr(VM *, Program *);
void execute(VM *);
	Program *exampleprogram(VM *);
int8 map(Opcode);
VM * virtualmachine(void);
int main(int, char**);