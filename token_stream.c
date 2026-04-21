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

void token_stream_dump(token_stream_t *ts)
{
  token_t tok = token_stream_peek(ts);
  while (tok.type != TOK_EOF)
  {
    switch (tok.type)
    {
    case TOK_INT:           printf("%d\n", tok.val.integer);  break;
    case TOK_FLOAT:         printf("%f\n", tok.val.floating); break;
    case TOK_IDENT:         printf("%s\n", tok.val.ident);   free(tok.val.ident); break;
    case TOK_STRING:        printf("\"%s\"\n", tok.val.str); free(tok.val.str);   break;
    case TOK_IF:            printf("IF\n");     break;
    case TOK_ELSE:          printf("ELSE\n");   break;
    case TOK_WHILE:         printf("WHILE\n");  break;
    case TOK_FOR:           printf("FOR\n");    break;
    case TOK_FN:            printf("FN\n");     break;
    case TOK_RETURN:        printf("RETURN\n"); break;
    case TOK_OPEN_PAREN:    printf("(\n");      break;
    case TOK_CLOSE_PAREN:   printf(")\n");      break;
    case TOK_OPEN_SQUARE:   printf("[\n");      break;
    case TOK_CLOSE_SQUARE:  printf("]\n");      break;
    case TOK_OPEN_CURLY:    printf("{\n");      break;
    case TOK_CLOSE_CURLY:   printf("}\n");      break;
    case TOK_SEMICOLON:     printf(";\n");      break;
    case TOK_COMMA:         printf(",\n");      break;
    case TOK_PLUS:          printf("+\n");      break;
    case TOK_MINUS:         printf("-\n");      break;
    case TOK_STAR:          printf("*\n");      break;
    case TOK_DIV:           printf("/\n");      break;
    case TOK_NOT:           printf("!\n");      break;
    case TOK_AND:           printf("&&\n");     break;
    case TOK_OR:            printf("||\n");     break;
    case TOK_ASSIGN:        printf("=\n");      break;
    case TOK_EQUAL:         printf("==\n");     break;
    case TOK_NOT_EQUAL:     printf("!=\n");     break;
    case TOK_LT:            printf("<\n");      break;
    case TOK_GT:            printf(">\n");      break;
    default:                printf("TOK_UNKNOWN\n"); break;
    }

    token_stream_advance(ts);
    tok = token_stream_peek(ts);
  }
}
