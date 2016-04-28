#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// 8 registers are in upper part of memory
extern uint16_t memory[];
extern uint16_t* reg;

extern unsigned long cycle_count;

uint32_t read_block(FILE* p);
void instr_set(FILE* p); // 1
void instr_push(FILE* p);
int instr_pop(FILE* p);
void instr_eq(FILE* p);
void instr_gt(FILE* p);
void instr_jmp(FILE* p);
void instr_add(FILE* p);
void instr_mult(FILE* p);
void instr_mod(FILE* p);
void instr_and(FILE* p);
void instr_or(FILE* p);
void instr_not(FILE* p);
void instr_rmem(FILE* p);
void instr_wmem(FILE* p);
void instr_out(FILE* p);
void instr_noop(FILE* p); //21

void memory_print();
void register_print();
#endif
