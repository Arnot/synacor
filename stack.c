#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

int stack_push(uint16_t value)
{
  uint16_t* new_data;

  /* printf("Adding %x (%d) to stack.\n", value, value); */
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

uint16_t stack_pop()
{
  uint16_t *new_data;
  uint16_t ret = 0;

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
  /* printf("Popping %x (%d) from stack.\n", ret, ret); */

  return ret;
}

void stack_print()
{
  int i;

  printf("Stack size is %d\n", stack.size);
  for (i = 0; i < stack.size; i++)
    printf("%d ", stack.data[i]);
  printf("\n");
}
