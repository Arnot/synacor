#ifndef __STACK_H__
#define __STACK_H__

#define STACK_MIN_SIZE 64

struct Stack {
  int* data;
  int size;
  int max_size;
} stack;

int stack_init();
void stack_destroy();
int stack_push(int value);
int stack_pop();
void stack_print();

#endif
