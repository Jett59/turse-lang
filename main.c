#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 1024

struct stack {
  size_t top;
  double stack[STACK_SIZE];
};

typedef void (*Handler)(struct stack *);

#define PUSH(STACK, VALUE)                                                     \
  do {                                                                         \
    if ((STACK)->top == STACK_SIZE) {                                          \
      fprintf(stderr, "Stack overflow\n");                                     \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    (STACK)->stack[(STACK)->top++] = (VALUE);                                  \
  } while (0)
#define POP(STACK)                                                             \
  ((STACK)->top > 0                                                            \
       ? (STACK)->stack[--(STACK)->top]                                        \
       : (fprintf(stderr, "Stack underflow\n"), exit(EXIT_FAILURE), 0))

static void handle0(struct stack *stack) { PUSH(stack, 0); }
static void handle1(struct stack *stack) { PUSH(stack, 1); }
static void handle2(struct stack *stack) { PUSH(stack, 2); }
static void handle3(struct stack *stack) { PUSH(stack, 3); }
static void handle4(struct stack *stack) { PUSH(stack, 4); }
static void handle5(struct stack *stack) { PUSH(stack, 5); }
static void handle6(struct stack *stack) { PUSH(stack, 6); }
static void handle7(struct stack *stack) { PUSH(stack, 7); }
static void handle8(struct stack *stack) { PUSH(stack, 8); }
static void handle9(struct stack *stack) { PUSH(stack, 9); }
static void handle10(struct stack *stack) { PUSH(stack, 10); }
static void handle11(struct stack *stack) { PUSH(stack, 11); }
static void handle12(struct stack *stack) { PUSH(stack, 12); }
static void handle13(struct stack *stack) { PUSH(stack, 13); }
static void handle14(struct stack *stack) { PUSH(stack, 14); }
static void handle15(struct stack *stack) { PUSH(stack, 15); }
static void handlePlus(struct stack *stack) {
  PUSH(stack, POP(stack) + POP(stack));
}
static void handleMinus(struct stack *stack) {
  PUSH(stack, POP(stack) - POP(stack));
}
static void handleMultiply(struct stack *stack) {
  PUSH(stack, POP(stack) * POP(stack));
}
static void handleDivide(struct stack *stack) {
  PUSH(stack, POP(stack) / POP(stack));
}
static void handleHexAppend(struct stack *stack) {
  PUSH(stack, POP(stack) * 16 + POP(stack));
}
static void handleDuplicate(struct stack *stack) {
  double value = POP(stack);
  PUSH(stack, value);
  PUSH(stack, value);
}
static void handlePrintChar(struct stack *stack) { putchar((int)POP(stack)); }
static void handlePrintHexChar(struct stack *stack) {
  handleHexAppend(stack);
  handlePrintChar(stack);
}

static Handler handlers[CHAR_MAX + 1] = {
    ['0'] = handle0,         ['1'] = handle1,
    ['2'] = handle2,         ['3'] = handle3,
    ['4'] = handle4,         ['5'] = handle5,
    ['6'] = handle6,         ['7'] = handle7,
    ['8'] = handle8,         ['9'] = handle9,
    ['a'] = handle10,        ['b'] = handle11,
    ['c'] = handle12,        ['d'] = handle13,
    ['e'] = handle14,        ['f'] = handle15,
    ['+'] = handlePlus,      ['-'] = handleMinus,
    ['*'] = handleMultiply,  ['/'] = handleDivide,
    ['~'] = handleHexAppend, [','] = handleDuplicate,
    ['|'] = handlePrintChar, ['\\'] = handlePrintHexChar};

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Missing file operand\n");
    return EXIT_FAILURE;
  }
  FILE *inputStream = fopen(argv[1], "r");
  fseek(inputStream, 0, SEEK_END);
  size_t inputSize = ftell(inputStream);
  rewind(inputStream);
  char *input = malloc(inputSize * sizeof(char));
  inputSize = fread(input, sizeof(char), inputSize, inputStream);
  fclose(inputStream);
  struct stack stack;
  stack.top = 0;
  for (size_t offset = 0; offset < inputSize; offset++) {
    char c = input[offset];
    if (c <=CHAR_MAX && handlers[c]) {
      handlers[c](&stack);
    }else {
      fprintf(stderr, "Unknown character %c\n", c);
      free(input);
      return EXIT_FAILURE;
    }
  }
  free(input);
  return 0;
}
