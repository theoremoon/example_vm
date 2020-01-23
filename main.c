#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define REG_SIZE 4
#define MEM_SIZE 0x100

#define OPCODE 0
#define OPERAND1 4
#define OPERAND2 8

typedef struct {
    uint32_t opcode;
    uint32_t operand1;
    uint32_t operand2;
} Instruction;


typedef struct {
    uint32_t regs[REG_SIZE];
    uint32_t mem[MEM_SIZE];
    uint32_t pc;
} VMState;

enum {
    OP_ADD,
    OP_SUB,
    OP_LOAD,
    OP_STORE,
    OP_MOV,
    OP_MOVI,
    OP_XOR,
    OP_AND,
    OP_OR,
    OP_PUTCHAR,
    OP_GETCHAR,
    OP_JMP,
    OP_JZ,
    OP_HALT,
};

void vmrun(Instruction* insts, VMState* vm) {
    uint32_t operand1;
    uint32_t operand2;
    Instruction instr;

    for (;;) {
        instr = insts[vm->pc];
        switch (instr.opcode) {
            case OP_ADD:
                operand1 = vm->regs[instr.operand1];
                operand2 = vm->regs[instr.operand2];
                vm->regs[instr.operand1] = operand1 + operand2;
                break;
            case OP_SUB:
                operand1 = vm->regs[instr.operand1];
                operand2 = vm->regs[instr.operand2];
                vm->regs[instr.operand1] = operand1 - operand2;
                break;
            case OP_MOV:
                operand2 = vm->regs[instr.operand2];
                vm->regs[instr.operand1] = operand2;
                break;
            case OP_MOVI:
                operand2 = instr.operand2;
                vm->regs[instr.operand1] = operand2;
                break;
            case OP_LOAD:
                operand2 = vm->mem[instr.operand2];
                vm->regs[instr.operand1] = operand2;
                break;
            case OP_STORE:
                operand2 = vm->regs[instr.operand2];
                vm->mem[instr.operand1] = operand2;
                break;
            case OP_XOR:
                operand1 = vm->regs[instr.operand1];
                operand2 = vm->regs[instr.operand2];
                vm->regs[instr.operand1] = operand1 ^ operand2;
                break;
            case OP_AND:
                operand1 = vm->regs[instr.operand1];
                operand2 = vm->regs[instr.operand2];
                vm->regs[instr.operand1] = operand1 & operand2;
                break;
            case OP_OR:
                operand1 = vm->regs[instr.operand1];
                operand2 = vm->regs[instr.operand2];
                vm->regs[instr.operand1] = operand1 | operand2;
                break;
            case OP_PUTCHAR:
                operand1 = vm->regs[instr.operand1];
                write(0, &operand1, sizeof(operand1));
                break;
            case OP_GETCHAR:
                read(0, &(vm->regs[instr.operand1]), sizeof(vm->regs[instr.operand1]));
                break;
            case OP_JMP:
                operand1 = instr.operand1;
                vm->pc = operand1;
                break;
            case OP_JZ:
                operand2 = vm->regs[instr.operand2];
                if (operand2 == 0) {
                    vm->pc = instr.operand1;
                }
                break;
            case OP_HALT:
                return;

            default:
                /* NOP */
                break;
        }
        vm->pc++;
    }
}

int main(int argc, char **argv) {
    VMState vm = {};
    Instruction *instructions;
    int fd;
    struct stat stbuf;

    if (argc == 1) {
        printf("%s <instructions>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    if (fstat(fd, &stbuf) == -1) {
        perror("fstat");
        return 1;
    }

    instructions = (Instruction*)malloc(stbuf.st_size);
    if (instructions == NULL) {
        perror("malloc");
        return 1;
    }
    if (stbuf.st_size != read(fd, instructions, stbuf.st_size)) {
        perror("read");
        return 1;
    }

    vmrun(instructions, &vm);

    close(fd);
}
