#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stack.h"
#include "cpu.h"

// 8 registers are in upper part of memory
uint16_t memory[32768+8] = {0};
uint16_t* reg = &memory[32768];

unsigned long cycle_count;

int main(int argc, char* argv[])
{
  FILE* program;
  int halted = 0;
  uint16_t opcode;
  int ret;

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

  while (!halted) {
    cycle_count++;
    opcode = read_block(program);
    //printf("opcode: %u\n", opcode);
    switch (opcode) {
    case 0: // halt
      halted = 1;
      break;

    case 1: // set
      instr_set(program);
      break;

    case 2: // push
      instr_push(program);
      break;

    case 3: // pop
      ret = instr_pop(program);
      if (ret != 0) {
        printf("Trying to pop empty stack!\n");
        halted = 1;
      }
      break;

    case 4: // eq
      instr_eq(program);
      break;

    case 5: // gt
      instr_gt(program);
      break;

    case 6: // jmp
      instr_jmp(program);
      break;

    case 7:
      instr_jt(program);
      break;

    case 8:
      instr_jf(program);
      break;

    case 9: // add
      instr_add(program);
      break;

    case 10: // mult
      instr_mult(program);
      break;

    case 11: // mod
      instr_mod(program);
      break;

    case 12: // and
      instr_and(program);
      break;

    case 13: // or
      instr_or(program);
      break;

    case 14: // not
      instr_not(program);
      break;

    case 15: // rmem
      instr_rmem(program);
      break;

    case 16: // wmem
      instr_wmem(program);
      break;

    case 19: //out
      instr_out(program);
      break;

    case 21: // noop
      break;

    default:
      printf("\nUnidentified opcode (%u) or EOF, halting\n", opcode);
      halted = 1;
      break;
    }
  }

//  memory_print();
  register_print();
  printf("\n\nCycle count = %lu\n", cycle_count);
  fclose(program);
  stack_destroy();
  return 0;
}
