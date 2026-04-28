// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "../lang.h"
#include "micro-tests.h"
#include <string.h>

TEST(token_stream_tests, hello_world)
{
  token_t tok;
  token_stream_t ts;
  token_stream_init_from_input(&ts, "fn main() { printf(\"Hello, World!\"); }");
  
  tok = token_stream_peek(&ts);  
  ASSERT(tok.type == TOK_FN);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_IDENT);
  ASSERT(strcmp(tok.val.str, "main") == 0);
  free_tok(&tok);
  token_stream_advance(&ts);

  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_OPEN_PAREN);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_CLOSE_PAREN);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_OPEN_CURLY);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);  
  ASSERT(tok.type == TOK_IDENT);
  ASSERT(strcmp(tok.val.str, "printf") == 0);
  free_tok(&tok);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_OPEN_PAREN);
  token_stream_advance(&ts);
    
  tok = token_stream_peek(&ts);  
  ASSERT(tok.type == TOK_STRING);
  ASSERT(strcmp(tok.val.str, "Hello, World!") == 0);
  free_tok(&tok);
  token_stream_advance(&ts);
    
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_CLOSE_PAREN);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_SEMICOLON);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_CLOSE_CURLY);
  token_stream_advance(&ts);
  
  tok = token_stream_peek(&ts);
  ASSERT(tok.type == TOK_EOF);
  token_stream_advance(&ts);  
  
  TEST_SUCCESS;
}
