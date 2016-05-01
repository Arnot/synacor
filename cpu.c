#include "cpu.h"
#include "stack.h"

uint32_t read_block(FILE* p)
{
  int lo, hi;
  uint16_t value;

  lo = fgetc(p);
  hi = fgetc(p);
  if (!(lo > 255 || hi > 255)) {
    value = ((hi << 8) | lo) & 0xFFFF;
  } else {
    printf("\nEOF found\n");
    return 0xFFFFFFFF;
  }

  return value;
}

// Source operands need extra checking, as they can also come from registers
uint32_t read_src()
{
  uint32_t raw = memory[pc];

  pc++;
  // Check if it's a value from a register
  if (raw >= 32768 && raw < 32776) {
    return memory[raw];
  } else if (raw < 32768){
    return raw;
  } else {
    /* Invalid value */
    return 0xFFFFFFFF;
  }
}

uint32_t read_dest()
{
  uint32_t value = memory[pc];

  pc++;
  if (value > 32776)
    return 0xFFFFFFFF;
  else
    return value;
}

void instr_set() {
  uint16_t a, b;
  a = read_dest();
  b = read_src();
  memory[a] = b;
}

void instr_push() {
  uint16_t a;
  a = read_src();
  stack_push(a);
}

int instr_pop() {
  uint16_t a, tmp;
  a = read_dest();
  tmp = stack_pop();
  if (tmp < 0) {
    printf("Popped empty stack!\n");
    return 1;
  }
  memory[a] = tmp;
  return 0;
}

void instr_eq() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  printf("eq - %x == %x at addr 0x%x\n", b, c, pc-8);
  memory[a] = (b == c);
}

void instr_gt() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  memory[a] = (b > c);
}

void instr_jmp() {
  uint16_t a;
  a = read_src();
  printf("jumping from %x to %x\n", pc, a);
  pc = a;
}

void instr_jt() {
  uint16_t a, b;
  a = read_src();
  b = read_src();
  if (a != 0) {
    printf("jt'ing from %x to %x, a = %d\n", pc, b, a);
    pc = b;
  }
}

void instr_jf() {
  uint16_t a, b;
  a = read_src();
  b = read_src();
  if (a == 0) {
    printf("jf'ing from %x to %x, a = %d\n", pc, b, a);
    pc = b;
  }
}

void instr_add() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  memory[a] = (b + c) % 32768;
}

void instr_mult() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  memory[a] = (b * c) % 32768;
}

void instr_mod() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  memory[a] = (b % c);
}

void instr_and() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  memory[a] = (b & c) % 32768;
}

void instr_or() {
  uint16_t a, b, c;
  a = read_dest();
  b = read_src();
  c = read_src();
  memory[a] = (b | c) % 32768;
}

void instr_not() {
  uint16_t a, b;
  a = read_dest();
  b = read_src();
  memory[a] = ~b & 0x7FFF;
}

void instr_rmem() {
  uint16_t a, b;
  a = read_dest();
  b = read_src();
  memory[a] = memory[b];
  printf("rmem: memory[0x%x] = memory[0x%x] (= %x)\n", a, b, memory[b]);
}

void instr_wmem() {
  uint16_t a, b;
  a = read_dest();
  b = read_src();
  memory[a] = b;
}

void instr_call() {
  uint16_t a;
  a = read_src();
  printf("Pushing 0x%x to the stack, jumping to 0x%x\n", pc, a);
  stack_push(pc);
  pc = a;
}

void instr_ret() {
  uint16_t addr;
  addr = stack_pop();
  printf("returning to addr 0x%x\n", addr);
  pc = addr;
}

void instr_out() {
  uint16_t a;
  a = read_src();
  printf("%c", a);
}

void instr_noop() {

}

void memory_print()
{
  int i, j;
  printf("\n");
  for (i = 0; i < 2048; i++) {
    for (j = 0; j < 16; j++) {
      printf("%4x ", memory[j+i*32]);
    }
    printf("\n");
  }
}

void register_print()
{
  int i;
  for (i = 0; i < 8; i++) {
    printf("reg %d: %u (%x)\n", i, reg[i], reg[i]);
  }
}
