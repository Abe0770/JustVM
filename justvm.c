/* justvm */
#include "justvm.h"
#include <string.h>

void __ste(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x08;
}
void __stg(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x04;
}
void __sth(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x02;
}
void __stl(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags |= 0x01;
}

void __cle(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x07;
}
void __clg(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0c;
}
void __clh(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0d;
}
void __cll(VM *vm, Opcode opcode, Args a1, Args a2) {
    vm $flags &= 0x0e;
}

void __mov(VM *vm, Opcode opcode, Args a1, Args a2) {
    int16 dst;

    dst = $2 a1;

    if(higher(vm) && lower(vm))
        error(vm, ErrInstr);

    switch(opcode) {
        /* mov ax   1000 */
        case 0x08:
            if(higher(vm))
                vm $ax =  (((Reg)a1 << 8) | (vm $ax & 0xFF));
            else if(lower(vm))
                vm $ax =  ((Reg)a1 | (vm $ax & 0xFF00));
            else 
                vm $ax = (Reg)a1;

            break;

        /* mov bx   1001 */
        case 0x09:
            if(higher(vm))
                vm $bx =  (((Reg)a1 << 8) | (vm $bx & 0xFF));
            else if(lower(vm))
                vm $bx =  ((Reg)a1 | (vm $bx & 0xFF00));
            else 
                vm $bx = (Reg)a1;
            break;

        /* mov cx   1010 */
        case 0x0a:
            if(higher(vm))
                vm $cx =  (((Reg)a1 << 8) | (vm $cx & 0xFF));
            else if(lower(vm))
                vm $cx =  ((Reg)a1 | (vm $cx & 0xFF00));
            else 
                vm $cx = (Reg)a1;
            break;

        /* mov dx   1011 */
        case 0x0b:
            if(higher(vm))
                vm $dx =  (((Reg)a1 << 8) | (vm $dx & 0xFF));
            else if(lower(vm))
                vm $dx =  ((Reg)a1 | (vm $dx & 0xFF00));
            else 
                vm $dx = (Reg)a1;
            break;

        /* mov sp   1100 */
        case 0x0c:
                vm $sp = (Reg)dst;
            break;

        // /* 1101 */
        // case 0x0d:=
        //     break;

        // /* 1110 */
        // case 0x0e:
        //     break;
        
        /* mov [addr]   1111 */
        case 0x0f:
            /* (xx) */
            break;

        default:
            error(vm, ErrInstr);
            break;
    }

    return;
}

void execinstr(VM *vm, Program *p) {
    int16 size;
    Args a1, a2;

    a1 = a2 = 0;
    size = map(*p);

    switch(size) {
        case 1:
            break;
        case 2:
            a1 = *(p + 1);
            break;
        case 3:
            a1 = ((((int16) * (p + 2) & 0xff) << 8) | ((int16) * (p + 1) & 0xff));
            break;
        case 5:
            a1 = ((((int16) * (p + 2) & 0xff) << 8) | ((int16) * (p + 1) & 0xff));
            a2 = ((((int16) * (p + 4) & 0xff) << 8) | ((int16) * (p + 3) & 0xff));
            break;
        default:
            segfault(vm);
            break;
    }

    switch(*p) {
        case mov:
        case 0x09 ... 0x0f:
            __mov(vm, (Opcode)*p, a1, a2);
            break;
        case nop:
            break;
        case hlt:
            error(vm, SysHlt);
            break;
        case ste: __ste(vm, (Opcode)*p, a1, a2); break;
        case stg: __stg(vm, (Opcode)*p, a1, a2); break;
        case sth: __sth(vm, (Opcode)*p, a1, a2); break;
        case stl: __stl(vm, (Opcode)*p, a1, a2); break;
        case cle: __cle(vm, (Opcode)*p, a1, a2); break;
        case clg: __clg(vm, (Opcode)*p, a1, a2); break;
        case clh: __clh(vm, (Opcode)*p, a1, a2); break;
        case cll: __cll(vm, (Opcode)*p, a1, a2); break;
    }

    return;
}

void execute(VM *vm) {
    int32 brkaddr;
    Program *pp;
    int16 size;
    Instruction ip;

    assert(vm && *vm -> m);
    size = 0;
    brkaddr = ((int32)vm -> m + vm -> b);
    pp = (Program *)&vm -> m;

    // instr1 arg1 instr2 instr3
    // mov ax, 0x05; nop; hlt;
    // 0x01 0x00 0x05
    // 0x02 
    // 0x03


    do {
        vm $ip += size;
        pp += size;

        if((int32)pp > brkaddr)
            segfault(vm);

        size = map(*pp);
        execinstr(vm, pp);

    } while(*pp != (Opcode)hlt);
    return;
}

void error(VM *vm, Errorcode e) {
    int8 exitcode;
    exitcode = -1;
    
    switch(e) {
        case ErrSegv:
            fprintf(stderr, "%s\n", "VM Segmentation fault \n");
            break;
        case ErrInstr:
            fprintf(stderr, "%s\n", "VM Illegal instruction \n");
            break;

        /* (xx) */
        case SysHlt:
            fprintf(stderr, "%s\n", "Sysem halted");
            exitcode = 0;
            printf("ax = %.04hx\n", $i vm $ax);
            if(equal(vm))
                printf("E flag set\n");
            if(gt(vm))
                printf("GT flag set\n");
            break;

        default:
            break;
    }

    if(vm)
        free(vm);

    exit($i exitcode);
}

int8 map(Opcode o) {
    int8 n, ret;
    IM *p;

    ret = 0;
    for(n = IMs, p = instrmap; n; n--, p++)
        if (p->o == o) {
            ret = p->s;
            break;
        }

    return ret;
}

VM *virtualmachine() {
    VM *p;
    int16 size;

    size = $2 sizeof(VM);

    p = (VM *) malloc($i size);
    if(!p) {
        errno = ErrMem;
        return (VM *)0;
    }
    zero($1 p, size);
    // memset($1 p, 0, size);

    return p;
}

Program *exampleprogram(VM *vm) {
    Program *p;
    Instruction *i0, *i1, *i2, *i3, *i4, *i5;
    Args a1;
    int16 s1, s2, sa1;

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);


    i0 = (Instruction *)malloc(s2);
    i1 = (Instruction *)malloc($i s1);
    i2 = (Instruction *)malloc(s2);
    i3 = (Instruction *)malloc(s2);
    i4 = (Instruction *)malloc(s2);
    i5 = (Instruction *)malloc(s2);
    assert(i1 && i2);
    zero($1 i0, s1);
    zero($1 i1, s1);
    zero($1 i2, s2);
    zero($1 i3, s2);
    zero($1 i4, s2);
    zero($1 i5, s2);

    i0 -> o = sth;
    p = vm -> m;
    copy($1 p, $1 i0, 1);
    ++p;

    vm $ax = 1;
    i1 -> o = 0x08; // mov
    sa1 = (s1 - 1);
    a1 = 0x02;
    // 0000 1010 mov cx
    // 0000 0000
    // 0000 0005 mov eax, 0x05

    copy($1 p, $1 i1, 1);
    ++p;

    if(a1) {
        copy($1 p, $1 &a1, sa1);
        p += sa1;
    }

    i2 -> o = nop;
    copy($1 p, $1 i2, 1);

    ++p;
    i3 -> o = ste;
    copy($1 p, $1 i3, 1);

    p++;
    i4 -> o = cll;
    copy($1 p, $1 i4, 1);

    p++;
    i5 -> o = hlt;
    copy($1 p, $1 i5, 1);

    vm -> b = (s1 + sa1 + s2 + s2);
    vm $ip = (Reg)vm -> m;
    vm $sp = (Reg) - 1; 
    free(i1);
    free(i2);


    return (Program *)&vm -> m;
}

int main(int argc, char *argv[]) {
    Program *prog;
    VM *vm;
    
    vm = virtualmachine();
    printf("vm = %p (size : %ld)\n", vm, sizeof(VM));

    prog = exampleprogram(vm); 
    printf("prog = %p\n", prog);

    execute(vm);

    printhex($1 prog, (map(mov) + map(nop) + map(hlt)), ' '); // 01 00 05 02 - mov 5 nop
    int sss;
    return 0;
}

#pragma GCC diagnostic pop


/*
0000 0001 - mov to ax
0000 0011 - mov to bx
0000 0101 - mov to cx
0000 0111 - mov to dx
0000 1111 - mov to memory address instead of registers

1111 - 15 - mov [0xaabb], 0x05


*/