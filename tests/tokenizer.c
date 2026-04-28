// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "../lang.h"
#include "micro-tests.h"
#include <string.h>

TEST(tokenizer_tests, hello_world)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("fn main() { printf(\"Hello, World!\"); }");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_FN);
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_IDENT);
  ASSERT(strcmp(tok.val.str, "main") == 0);
  free_tok(&tok);

  tok = next_tok(&t);
  ASSERT(tok.type == TOK_OPEN_PAREN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_CLOSE_PAREN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_OPEN_CURLY);
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_IDENT);
  ASSERT(strcmp(tok.val.str, "printf") == 0);
  free_tok(&tok);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_OPEN_PAREN);
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_STRING);
  ASSERT(strcmp(tok.val.str, "Hello, World!") == 0);
  free_tok(&tok);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_CLOSE_PAREN);

  tok = next_tok(&t);
  ASSERT(tok.type == TOK_SEMICOLON);

  tok = next_tok(&t);
  ASSERT(tok.type == TOK_CLOSE_CURLY);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}
