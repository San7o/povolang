// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void token_stream_init(token_stream_t *ts)
{
  if (!ts) return;

  ts->tokens   = NULL;
  ts->size     = 0;
  ts->capacity = 0;
  ts->pos      = 0;
}

void token_stream_add(token_stream_t *ts, token_t tok)
{
  if (!ts) return;

  if (!ts->tokens)
  {
    ts->tokens   = malloc(sizeof(token_t) * 8);
    ts->capacity = 8;

    ts->tokens[0] = tok;
    ts->size      = 1;
    return;
  }

  if (ts->size >= ts->capacity)
  {
    // Double the vector
    token_t *new_vec = malloc(sizeof(token_t) * ts->capacity * 2);
    memcpy(new_vec, ts->tokens, ts->size * sizeof(token_t));
    free(ts->tokens);
    ts->tokens = new_vec;
    ts->capacity *= 2;
  }
  
  ts->tokens[ts->size] = tok;
  ts->size++;
  return;
}

token_t token_stream_peek(token_stream_t *ts)
{
  if (!ts || ts->pos == ts->size)
    return (token_t) { .type = TOK_EOF };
  return ts->tokens[ts->pos];
}

void token_stream_advance(token_stream_t *ts)
{
  if (!ts) return;
  ts->pos++;
}

void token_stream_init_from_input(token_stream_t *ts, char *input)
{
  if (!ts) return;

  token_stream_init(ts);
  tokenizer_t t = init_tok(input);

  token_t tok = next_tok(&t);
  while (tok.type != TOK_EOF)
  {
    token_stream_add(ts, tok);
    tok = next_tok(&t);
  }

  return;
}

void token_dump(token_t tok)
{
  switch (tok.type)
  {
  case TOK_INT:           printf("%d", tok.val.integer);  break;
  case TOK_FLOAT:         printf("%f", tok.val.floating); break;
  case TOK_IDENT:         printf("%s", tok.val.ident);   free(tok.val.ident); break;
  case TOK_STRING:        printf("\"%s\"", tok.val.str); free(tok.val.str);   break;
  case TOK_IF:            printf("IF");     break;
  case TOK_ELSE:          printf("ELSE");   break;
  case TOK_WHILE:         printf("WHILE");  break;
  case TOK_FOR:           printf("FOR");    break;
  case TOK_FN:            printf("FN");     break;
  case TOK_RETURN:        printf("RETURN"); break;
  case TOK_OPEN_PAREN:    printf("(");      break;
  case TOK_CLOSE_PAREN:   printf(")");      break;
  case TOK_OPEN_SQUARE:   printf("[");      break;
  case TOK_CLOSE_SQUARE:  printf("]");      break;
  case TOK_OPEN_CURLY:    printf("{");      break;
  case TOK_CLOSE_CURLY:   printf("}");      break;
  case TOK_SEMICOLON:     printf(";");      break;
  case TOK_COMMA:         printf(",");      break;
  case TOK_PLUS:          printf("+");      break;
  case TOK_MINUS:         printf("-");      break;
  case TOK_STAR:          printf("*");      break;
  case TOK_DIV:           printf("/");      break;
  case TOK_NOT:           printf("!");      break;
  case TOK_AND:           printf("&&");     break;
  case TOK_OR:            printf("||");     break;
  case TOK_ASSIGN:        printf("=");      break;
  case TOK_EQUAL:         printf("==");     break;
  case TOK_NOT_EQUAL:     printf("!=");     break;
  case TOK_LT:            printf("<");      break;
  case TOK_GT:            printf(">");      break;
  default:                printf("TOK_UNKNOWN"); break;
  }
}

void token_stream_dump(token_stream_t *ts)
{
  token_t tok = token_stream_peek(ts);
  while (tok.type != TOK_EOF)
  {
    token_dump(tok);
    printf("\n");
    token_stream_advance(ts);
    tok = token_stream_peek(ts);
  }
}
