#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_MIN_SIZE 32

/********************
 * Memory areas
 ********************/
uint16_t main_mem[32768];
uint16_t reg[8];
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

uint16_t read_block(FILE* p)
{
  char lo, hi;
  lo = fgetc(p);
  hi = fgetc(p);
  return ((hi << 8) | lo) % 32768;
}

int main(int argc, char* argv[])
{
  unsigned long cycle_count;
  FILE* program;
  int halted = 0;
  uint16_t opcode;
  uint16_t a, b;

  cycle_count = 0;
  if (stack_init() != 0) {
    printf("Failed to initialize stack.\n");
    return 1;
  }

  if (argc < 2) {
    printf("Too little command line arguments\n");
    return 1;
  }

  program = fopen(argv[1], "rb");
  if (program == NULL) {
    printf("Couldn't open challenge.bin for reading\n");
    exit(1);
  }

  while (!halted) {
    cycle_count++;
    opcode = read_block(program);
    switch (opcode) {
    // halt
    case 0: //printf("\nhalt\n");
            halted = 1;
            break;

    //out
    case 19: //printf("\nout\n");
             a = read_block(program);
             printf("%c", a);
             break;

    // noop
    case 21: //printf("\nnoop\n");
             break;

    default: printf("\nUnidentified opcode or EOF, halting\n");
             halted = 1;
             break;
    }
  }

  printf("\n\nCycle count = %lu\n", cycle_count);
  fclose(program);
  stack_destroy();
  return 0;
}
