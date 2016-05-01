#ifndef __STACK_H__
#define __STACK_H__
#include <stdint.h>

#define STACK_MIN_SIZE 64

struct Stack {
  uint16_t* data;
  int size;
  int max_size;
} stack;

int stack_init();
void stack_destroy();
int stack_push(uint16_t value);
uint16_t stack_pop();
void stack_print();

#endif
