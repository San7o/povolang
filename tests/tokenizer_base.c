// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "../lang.h"
#include "micro-tests.h"

#include <string.h>

TEST(tokenizer_tests, ident)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("hello");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_IDENT);

  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, integer)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("123");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_INT);
  ASSERT(tok.val.integer == 123);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, floating)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("123.123");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_FLOAT);
  ASSERT(tok.val.floating - 123.123f < 1e-5
         && tok.val.floating - 123.123f > -1e-5);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, string)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("\"hello\"");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_STRING);
  ASSERT(strcmp(tok.val.str, "hello") == 0);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, boolean_true)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("true");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_TRUE);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, boolean_false)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("false");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_FALSE);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, keyword_if)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("if");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_IF);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, keyword_else)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("else");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_ELSE);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, keyword_while)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("while");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_WHILE);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, keyword_for)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("for");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_FOR);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, keyword_fn)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("fn");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_FN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, keyword_return)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("return");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_RETURN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, open_paren)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("(");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_OPEN_PAREN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, close_paren)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok(")");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_CLOSE_PAREN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, open_square)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("[");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_OPEN_SQUARE);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, close_square)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("]");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_CLOSE_SQUARE);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, open_curly)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("{");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_OPEN_CURLY);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, close_curly)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("}");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_CLOSE_CURLY);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, semicolon)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok(";");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_SEMICOLON);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, comma)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok(",");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_COMMA);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, plus)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("+");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_PLUS);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, minus)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("-");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_MINUS);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, star)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("*");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_STAR);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, div)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("/");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_DIV);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, not)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("!");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_NOT);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, and)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("&&");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_AND);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, or)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("||");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_OR);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, assign)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("=");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_ASSIGN);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, equal)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("==");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_EQUAL);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, not_equal)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("!=");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_NOT_EQUAL);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, lt)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok("<");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_LT);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}

TEST(tokenizer_tests, gt)
{
  tokenizer_t t;
  token_t tok;

  t = init_tok(">");
  
  tok = next_tok(&t);  
  ASSERT(tok.type == TOK_GT);
  
  tok = next_tok(&t);
  ASSERT(tok.type == TOK_EOF);
  
  TEST_SUCCESS;
}
