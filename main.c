#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 1024

struct context {
  size_t stack_top;
  double stack[STACK_SIZE];
  char *input_buffer;
  size_t input_size;
  size_t current_offset;
};

typedef void (*Handler)(struct context *);

#define PUSH(CONTEXT, VALUE)                                                   \
  do {                                                                         \
    if ((CONTEXT)->stack_top == STACK_SIZE) {                                  \
      fprintf(stderr, "Stack overflow\n");                                     \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    (CONTEXT)->stack[(CONTEXT)->stack_top++] = (VALUE);                        \
  } while (0)
#define POP(CONTEXT)                                                           \
  ((CONTEXT)->stack_top > 0                                                    \
       ? (CONTEXT)->stack[--(CONTEXT)->stack_top]                              \
       : (fprintf(stderr, "Stack underflow\n"), exit(EXIT_FAILURE), 0))

static void handle0(struct context *context) { PUSH(context, 0); }
static void handle1(struct context *context) { PUSH(context, 1); }
static void handle2(struct context *context) { PUSH(context, 2); }
static void handle3(struct context *context) { PUSH(context, 3); }
static void handle4(struct context *context) { PUSH(context, 4); }
static void handle5(struct context *context) { PUSH(context, 5); }
static void handle6(struct context *context) { PUSH(context, 6); }
static void handle7(struct context *context) { PUSH(context, 7); }
static void handle8(struct context *context) { PUSH(context, 8); }
static void handle9(struct context *context) { PUSH(context, 9); }
static void handle10(struct context *context) { PUSH(context, 10); }
static void handle11(struct context *context) { PUSH(context, 11); }
static void handle12(struct context *context) { PUSH(context, 12); }
static void handle13(struct context *context) { PUSH(context, 13); }
static void handle14(struct context *context) { PUSH(context, 14); }
static void handle15(struct context *context) { PUSH(context, 15); }
static void handlePlus(struct context *context) {
  PUSH(context, POP(context) + POP(context));
}
static void handleMinus(struct context *context) {
  PUSH(context, POP(context) - POP(context));
}
static void handleMultiply(struct context *context) {
  PUSH(context, POP(context) * POP(context));
}
static void handleDivide(struct context *context) {
  PUSH(context, POP(context) / POP(context));
}
static void handleHexAppend(struct context *context) {
  PUSH(context, POP(context) * 16 + POP(context));
}
static void handleDuplicate(struct context *context) {
  double value = POP(context);
  PUSH(context, value);
  PUSH(context, value);
}
static void handlePrintChar(struct context *context) {
  putchar((int)POP(context));
}
static void handlePrintHexChar(struct context *context) {
  handleHexAppend(context);
  handlePrintChar(context);
}
// Initialized to all -1.
static size_t customWords[CHAR_MAX + 1];
static void handleCustomWord(struct context *context) {
  if (context->input_size > ++context->current_offset) {
    char word = context->input_buffer[context->current_offset++];
    customWords[word] = context->current_offset;
    while (context->current_offset < context->input_size &&
           context->input_buffer[context->current_offset++] != ';')
      ;
    if (context->current_offset == context->input_size) {
      fprintf(stderr, "Unterminated custom word definition %c\n", word);
      exit(EXIT_FAILURE);
    }
    context->current_offset--;
  }
}
#define RETURN_STACK_SIZE 65536
static size_t returnStack[RETURN_STACK_SIZE];
static size_t returnStackTop = 0;
static void handleReturn(struct context *context) {
  if (returnStackTop > 0) {
    context->current_offset = returnStack[--returnStackTop];
  } else {
    fprintf(stderr, "Return without matching call\n");
    exit(EXIT_FAILURE);
  }
}

static Handler handlers[CHAR_MAX + 1] = {
    ['0'] = handle0,          ['1'] = handle1,
    ['2'] = handle2,          ['3'] = handle3,
    ['4'] = handle4,          ['5'] = handle5,
    ['6'] = handle6,          ['7'] = handle7,
    ['8'] = handle8,          ['9'] = handle9,
    ['a'] = handle10,         ['b'] = handle11,
    ['c'] = handle12,         ['d'] = handle13,
    ['e'] = handle14,         ['f'] = handle15,
    ['+'] = handlePlus,       ['-'] = handleMinus,
    ['*'] = handleMultiply,   ['/'] = handleDivide,
    ['~'] = handleHexAppend,  [','] = handleDuplicate,
    ['|'] = handlePrintChar,  ['\\'] = handlePrintHexChar,
    [':'] = handleCustomWord, [';'] = handleReturn};

int main(int argc, char **argv) {
  memset(customWords, 0xff, sizeof(customWords));
  if (argc < 2) {
    fprintf(stderr, "Missing file operand\n");
    return EXIT_FAILURE;
  }
  FILE *input_stream = fopen(argv[1], "r");
  fseek(input_stream, 0, SEEK_END);
  size_t input_size = ftell(input_stream);
  rewind(input_stream);
  char *input = malloc(input_size * sizeof(char));
  input_size = fread(input, sizeof(char), input_size, input_stream);
  fclose(input_stream);
  struct context context;
  context.stack_top = 0;
  context.input_buffer = input;
  for (size_t offset = 0; offset < input_size; offset++) {
    context.current_offset = offset;
    char c = input[offset];
    if (handlers[c]) {
      handlers[c](&context);
    } else if (customWords[c] != SIZE_MAX) {
      returnStack[returnStackTop++] = offset;
      context.current_offset = customWords[c] - 1;
    } else {
      fprintf(stderr, "Unknown character %c\n", c);
      free(input);
      return EXIT_FAILURE;
    }
    offset = context.current_offset;
  }
  free(input);
  return 0;
}
