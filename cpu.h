#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 32768
// 8 registers are in upper part of memory
#define MEM_REG_SIZE 32768+8

extern uint32_t pc;

extern uint16_t memory[];
extern uint16_t* reg;

extern unsigned long cycle_count;

uint32_t read_block(FILE* p);

void instr_set(); // 1
void instr_push();
int instr_pop();
void instr_eq();
void instr_gt();
void instr_jmp();
void instr_jt();
void instr_jf();
void instr_add();
void instr_mult();
void instr_mod();
void instr_and();
void instr_or();
void instr_not();
void instr_rmem();
void instr_wmem();
void instr_call();
void instr_ret();
void instr_out();
void instr_in();
void instr_noop(); //21

void memory_print();
void register_print();
#endif
