#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stack.h"
#include "cpu.h"

// Program counter
uint32_t pc;

uint16_t memory[MEM_REG_SIZE] = {0};
uint16_t* reg = &memory[MEM_SIZE];

unsigned long cycle_count;

int main(int argc, char* argv[])
{
  FILE* program;
  int halted = 0;
  uint16_t opcode;
  int ret, i;

  cycle_count = 0;

  if (argc < 2) {
    printf("Too little command line arguments\n");
    return 1;
  }

  if (stack_init() != 0) {
    printf("Failed to initialize stack.\n");
    return 1;
  }

  program = fopen(argv[1], "rb");
  if (program == NULL) {
    printf("Couldn't open %s for reading\n", argv[1]);
    exit(1);
  }

  uint32_t block;
  int mem_pos = 0;
  while ((block = read_block(program)) <= MEM_REG_SIZE && mem_pos < MEM_SIZE) {
    memory[mem_pos] = (uint16_t) block;
    mem_pos++;
  }
  fclose(program);

  printf("Loaded %d blocks into memory\n", mem_pos);

  pc = 0;

  while (!halted) {
    cycle_count++;
    opcode = memory[pc];
    pc++;
    switch (opcode) {
    case 0: // halt
      halted = 1;
      break;

    case 1: // set
      instr_set();
      break;

    case 2: // push
      instr_push();
      break;

    case 3: // pop
      ret = instr_pop();
      if (ret != 0) {
        printf("Trying to pop empty stack!\n");
        halted = 1;
      }
      break;

    case 4: // eq
      instr_eq();
      break;

    case 5: // gt
      instr_gt();
      break;

    case 6: // jmp
      instr_jmp();
      break;

    case 7: // jt
      instr_jt();
      break;

    case 8: // jf
      instr_jf();
      break;

    case 9: // add
      instr_add();
      break;

    case 10: // mult
      instr_mult();
      break;

    case 11: // mod
      instr_mod();
      break;

    case 12: // and
      instr_and();
      break;

    case 13: // or
      instr_or();
      break;

    case 14: // not
      instr_not();
      break;

    case 15: // rmem
      instr_rmem();
      break;

    case 16: // wmem
      instr_wmem();
      break;

    case 17: // call
      instr_call();
      break;

    case 18: // ret
      instr_ret();
      break;

    case 19: //out
      instr_out();
      break;

    case 21: // noop
      break;

    default:
      printf("\nUnidentified opcode (%u) or EOF, halting\n", opcode);
      halted = 1;
      break;
    }
  }

  /* memory_print(); */
  register_print();
  stack_print();

  printf("Cycle count = %lu\n", cycle_count);
  printf("Program counter was at %x\n", pc);
  printf("Context: \n");
  for (i = pc - 12; i < pc + 12; i++)
    printf("%4x ", i);
  printf("\n");
  for (i = pc - 12; i < pc + 12; i++)
    printf("%4x ", memory[i]);
  printf("\n");

  stack_destroy();
  return 0;
}
