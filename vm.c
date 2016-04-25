#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_MIN_SIZE 32

unsigned long cycle_count;

/********************
 * Memory areas
 ********************/
// 8 registers are in upper part of memory
uint16_t memory[32768+8] = {0};
uint16_t* reg = &memory[32768];
struct Stack {
  int* data;
  int size;
  int max_size;
} stack;

int stack_init()
{
  stack.size = 0;
  stack.data = malloc(STACK_MIN_SIZE*sizeof(*stack.data));

  if (stack.data != NULL) {
    stack.max_size = STACK_MIN_SIZE;
    return 0;
  } else {
    stack.max_size = 0;
    return 1;
  }
}

void stack_destroy()
{
  free(stack.data);
  stack.max_size = 0;
  stack.size = 0;
}

int stack_push(int value)
{
  int* new_data;

  // resize stack if size exceeds max_size
  if (stack.size == stack.max_size) {
    printf("Growing stack..\n");
    new_data = realloc(stack.data, stack.max_size*2 * sizeof(*stack.data));

    if (new_data == NULL) {
      printf("Failed to grow stack.\n");
      return 1;
    }

    stack.data = new_data;
    stack.max_size = stack.max_size*2;
  }

  stack.data[stack.size] = value;
  stack.size++;
  return 0;
}

int stack_pop()
{
  int *new_data;
  int ret = 0;

  if (stack.size > 0) {
    ret = stack.data[stack.size - 1];
    stack.size--;
  } else {
    ret = -1;
  }

  if (stack.size < stack.max_size/2 && stack.size >= STACK_MIN_SIZE) {
    printf("Shrinking stack..\n");
    new_data = realloc(stack.data, stack.max_size/2 * sizeof(*stack.data));
    if (new_data == NULL) {
      printf("Failed to shrink stack.\n");
      return ret; // TODO: Error handling? ret is still valid..
    }

    stack.data = new_data;
    stack.max_size = stack.max_size/2;
  }

  return ret;
}

void stack_print()
{
  int i;

  for (i = 0; i < stack.size; i++)
    printf("%d ", stack.data[i]);
  printf("\n");
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

uint32_t read_block(FILE* p)
{
  // Need ints to check for EOF
  int low, high;
  char lo, hi;
  uint16_t value;

  low = fgetc(p);
  high = fgetc(p);
  lo = (char)low;
  hi = (char)high;
  if (!(low == EOF || high == EOF))
    {
      value = ((hi << 8) | lo) % 32768;
    } else {
    printf("\nEOF found\n");
    return 0xFFFFFFFF;
  }

  if (value < 32776)
    return value;
  else {
    printf("Invalid value! Using 0. Value was %u. Cycle %lu\n", value, cycle_count);
    printf("lo: %x; hi: %x\n", lo, hi);
    return 0;
  }
}

int main(int argc, char* argv[])
{
  FILE* program;
  int halted = 0;
  uint16_t opcode;
  uint16_t a, b, c;
  int tmp;

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
      a = read_block(program);
      b = read_block(program);
      reg[a] = b;
      break;

    case 2: // push
      a = read_block(program);
      stack_push(a);
      break;

    case 3: // pop
      a = read_block(program);
      tmp = stack_pop();
      if (tmp < 0) {
        printf("Popped empty stack!\n");
        return 1;
      }
      memory[a] = tmp;
      break;

    case 4: // eq
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b == c);
      break;

    case 5: // gt
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b > c);
      break;

    case 9: // add
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b + c) % 32768;
      break;

    case 10: // mult
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b * c) % 32768;
      break;

    case 11: // mod
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b % c);
      break;

    case 12: // and
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b & c);
      break;

    case 13: // or
      a = read_block(program);
      b = read_block(program);
      c = read_block(program);
      memory[a] = (b | c);
      break;

    case 14: // not
      a = read_block(program);
      b = read_block(program);
      memory[a] = ~(b & 0x7FFF);
      break;

    case 15: // rmem --> Same as wmem?
      a = read_block(program);
      b = read_block(program);
      memory[a] = memory[b];
      break;

    case 16: // wmem
      a = read_block(program);
      b = read_block(program);
      memory[a] = memory[b];
      break;

    case 19: //out
      a = read_block(program);
      printf("%c", a);
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
