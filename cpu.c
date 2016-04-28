#include "cpu.h"
#include "stack.h"

uint32_t read_block(FILE* p)
{
  // Need ints to check for EOF
  int low, high;
  char lo, hi;
  uint16_t value;

  low = fgetc(p);
  high = fgetc(p);
  if (!(low > 255 || high > 255)) {
    value = ((high << 8) | low) % 32768;
  } else {
    printf("\nEOF found\n");
    return 0xFFFFFFFF;
  }

  if (value < 32776)
    return value;
  else {
    printf("Invalid value! Using 0. Value was %u. Cycle %lu\n", value, cycle_count);
    printf("low: %x; high: %x\n", low, high);
    printf("File pointer was at %lx\n", ftell(p));
    return 0;
  }
}

// Source operands need extra checking, as they can also come from registers
uint32_t read_src(FILE* p)
{
  uint32_t raw = read_block(p);
  // Check if it's a value from a register
  if (raw >= 32768 && raw < 32776) {
    return memory[raw];
  } else {
    return raw;
  }
}

uint32_t read_dest(FILE* p)
{
  return read_block(p);
}

void instr_set(FILE* p) {
  uint16_t a, b;
  a = read_dest(p);
  b = read_src(p);
  memory[a] = b;
}

void instr_push(FILE* p) {
  uint16_t a;
  a = read_src(p);
  stack_push(a);
}

int instr_pop(FILE* p) {
  uint16_t a, tmp;
  a = read_dest(p);
  tmp = stack_pop();
  if (tmp < 0) {
    printf("Popped empty stack!\n");
    return 1;
  }
  memory[a] = tmp;
  return 0;
}

void instr_eq(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  memory[a] = (b == c);
}

void instr_gt(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  memory[a] = (b > c);
}

void instr_jmp(FILE* p) {
  uint16_t a;
  int ret;
  /*
   * Although a is the destination of the jump, it is a 'source' for this
   * instruction
   */
  a = read_src(p);
  ret = fseek(p, a*2, SEEK_SET);
  if (ret != 0) {
    printf("Jumped out of file!\n");
    exit(1);
  }
}

void instr_jt(FILE* p) {
  uint16_t a, b;
  int ret;
  a = read_src(p);
  b = read_src(p);
  if (a != 0) {
    ret = fseek(p, b*2, SEEK_SET);
    if (ret != 0) {
      printf("Jumped out of file!\n");
      exit(1);
    }
  }
}

void instr_jf(FILE* p) {
  uint16_t a, b;
  int ret;
  a = read_src(p);
  b = read_src(p);
  if (a == 0) {
    ret = fseek(p, b*2, SEEK_SET);
    if (ret != 0) {
      printf("Jumped out of file!\n");
      exit(1);
    }
  }
}

void instr_add(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  // printf("reg[%u] = %u + %u\n", a, b, c);
  reg[a] = (b + c) % 32768;
}

void instr_mult(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  memory[a] = (b * c) % 32768;
}

void instr_mod(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  memory[a] = (b % c);
}

void instr_and(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  memory[a] = (b & c);
}

void instr_or(FILE* p) {
  uint16_t a, b, c;
  a = read_dest(p);
  b = read_src(p);
  c = read_src(p);
  memory[a] = (b | c);
}

void instr_not(FILE* p) {
  uint16_t a, b;
  a = read_dest(p);
  b = read_src(p);
  memory[a] = ~(b & 0x7FFF);
}

void instr_rmem(FILE* p) {
  uint16_t a, b;
  a = read_dest(p);
  b = read_src(p);
  memory[a] = memory[b];
}

void instr_wmem(FILE* p) {
  uint16_t a, b;
  a = read_dest(p);
  b = read_src(p);
  memory[a] = b;
}

void instr_out(FILE* p) {
  uint16_t a;
  a = read_src(p);
  printf("%c", a);
}

void instr_noop(FILE* p) {

}

void memory_print()
{
  int i, j;
  printf("\n");
  for (i = 0; i < 1024; i++) {
    for (j = 0; j < 32; j++) {
      printf("%u ", memory[j+i*32]);
    }
    printf("\n");
  }
}

void register_print()
{
  int i;
  for (i = 0; i < 8; i++) {
    printf("reg %d: %u\n", i, reg[i]);
  }
}
