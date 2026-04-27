// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"

int main(void)
{
  char *input = 
    "fn main() { "
    "  // comment!\n"
    "  printf(\"Hello, world! %f\\n\", 69.420);"
    "  if (x < 10) {"
    "    exit(0);"
    "  }"
    "  else {"
    "    exit(1);"
    "  }"
    "}";
  
  // token_stream_t ts;
  // token_stream_init_from_input(&ts, input);
  // token_stream_dump(&ts);

  parser_t p;
  parser_init_from_input(&p, input);
  ast_node_t *program = parse_program(&p);

  // ast_dump(program);

  gen_program(program, "test.bc");
  
  return 0;
}
