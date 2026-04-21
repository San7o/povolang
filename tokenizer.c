// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Utility functions

int isdigit(int c) { return c >= '0' && c <= '9'; }
int isalpha(int c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int isspace(int c) { return (c == '\n') || (c == '\t') || (c == ' '); }

int toint(int c) { if (!isdigit(c)) return 0; return c - '0'; }

tokenizer_t init_tok(char* input)
{
  tokenizer_t tokenizer = {
    .input  = input,
    .size   = strlen(input),
    .pos    = 0,
  };

  return tokenizer;
}

token_t next_tok(tokenizer_t *t)
{
  token_t tok = {
    .type    = TOK_EOF,
  };
  
  if (!t || t->pos >= t->size)
    return tok;
  
  char c = t->input[t->pos];

  while (isspace(c) && t->pos+1 < t->size)
  {
    t->pos++;
    c = t->input[t->pos];
  }

  // Check single chars with a switch
  switch (c)
  {
  case '(': tok.type = TOK_OPEN_PAREN;   t->pos++; return tok;
  case ')': tok.type = TOK_CLOSE_PAREN;  t->pos++; return tok;
  case '[': tok.type = TOK_OPEN_SQUARE;  t->pos++; return tok;
  case ']': tok.type = TOK_CLOSE_SQUARE; t->pos++; return tok;
  case '{': tok.type = TOK_OPEN_CURLY;   t->pos++; return tok;
  case '}': tok.type = TOK_CLOSE_CURLY;  t->pos++; return tok;
  case ';': tok.type = TOK_SEMICOLON;    t->pos++; return tok;
  case ',': tok.type = TOK_COMMA;        t->pos++; return tok;
  case '<': tok.type = TOK_LT;           t->pos++; return tok;
  case '>': tok.type = TOK_GT;           t->pos++; return tok;
  case '=': 
  {
    if (t->pos + 1 < t->size && t->input[t->pos+1] == '=')
    {
      tok.type = TOK_EQUAL;
      t->pos++;
      t->pos++;
      return tok;
    }
    
    tok.type = TOK_ASSIGN;
    t->pos++;
    return tok;
  }
  case '!': 
  {
    if (t->pos + 1 < t->size && t->input[t->pos+1] == '=')
    {
      tok.type = TOK_NOT_EQUAL;
      t->pos++;
      t->pos++;
      return tok;
    }
    
    tok.type = TOK_NOT;
    t->pos++;
    return tok;
  }
  case '+': tok.type = TOK_PLUS;         t->pos++; return tok;
  case '-': tok.type = TOK_MINUS;        t->pos++; return tok;
  case '*': tok.type = TOK_STAR;         t->pos++; return tok;
  case '&':
  {
    if (t->pos+1 >= t->size) break;
    if (t->input[t->pos+1] == '&')
    {
      tok.type = TOK_AND;
      t->pos += 2;
      return tok;
    }
    break;
  }
  case '|':
  {
    if (t->pos+1 >= t->size) break;
    if (t->input[t->pos+1] == '|')
    {
      t->pos += 2;
      tok.type = TOK_OR;
      return tok;
    }
    break;
  }
  case '"':
  {
    int start = t->pos + 1;
    int end   = start;
    bool escaped = false;
    while(end < t->size)
    {
      if (t->input[end] == '\\' && !escaped)
      {
        escaped = true;
        end++;
        continue;
      }
      if (t->input[end] == '"' && !escaped)
        break;

      escaped = false;
      end++;
    }
    if (end >= t->size)
    {
      fprintf(stderr, "Error: string never closed\n");
      exit(1);
    }

    char *str = malloc(end - start + 1);
    strncpy(str, &t->input[start], end - start);
    str[end - start] = '\0';

    t->pos = end + 1;
    tok.type = TOK_STRING;
    tok.val.str = str;
    return tok;
  }
  case '/':
  {
    if (t->pos + 1 < t->size && t->input[t->pos+1] == '/')
    {
      // Comment, skip entire line
      while (t->pos < t->size && t->input[t->pos] != '\n') t->pos++;
      if (t->pos < t->size) t->pos++;  // skip newline character
    }
    else
    {
      tok.type = TOK_DIV;
      t->pos++;
      return tok;
    }

    return next_tok(t);
  }
  default:
    break;
  }
  
  // Check number
  if (isdigit(c))
  {
    int num = 0;
    while (isdigit(c) && t->pos < t->size)
    {
      num = num * 10 + toint(c);
      t->pos++;
      c = t->input[t->pos];
    }
    if (c == '.' && t->pos+1 < t->size && isdigit(t->input[t->pos + 1]))
    {
      t->pos++;
      // Float
      float fnum = (float) num;
      float it   = 10.0f;
      c = t->input[t->pos];
      while (isdigit(c))
      {
        fnum += ((float)toint(c)) / it;
        it *= 10.0f;
        t->pos++;
        c = t->input[t->pos];
      }

      tok.type = TOK_FLOAT;
      tok.val.floating = fnum;
      return tok;
    }
    else
    {
      tok.type = TOK_INT;
      tok.val.integer = num;
      return tok;
    }
  }

  // Identifier
  int start = t->pos;
  while (isalnum(t->input[t->pos]) && t->pos < t->size) t->pos++;

  if (t->pos == t->size || t->pos == start) return tok;
  
  int len = t->pos - start;
  // Check for keywords
  if (len == 2 && strncmp(&t->input[start], "if", 2) == 0)
  {
    tok.type = TOK_IF;
    return tok;
  }
  if (len == 4 && strncmp(&t->input[start], "else", 4) == 0)
  {
    tok.type = TOK_ELSE;
    return tok;
  }
  if (len == 5 && strncmp(&t->input[start], "while", 5) == 0)
  {
    tok.type = TOK_WHILE;
    return tok;
  }
  if (len == 3 && strncmp(&t->input[start], "for", 3) == 0)
  {
    tok.type = TOK_FOR;
    return tok;
  }
  if (len == 2 && strncmp(&t->input[start], "fn", 2) == 0)
  {
    tok.type = TOK_FN;
    return tok;
  }
  if (len == 6 && strncmp(&t->input[start], "return", 6) == 0)
  {
    tok.type = TOK_RETURN;
    return tok;
  }
  if (len == 4 && strncmp(&t->input[start], "true", 4) == 0)
  {
    tok.type = TOK_TRUE;
    return tok;
  }
  if (len == 5 && strncmp(&t->input[start], "false", 5) == 0)
  {
    tok.type = TOK_FALSE;
    return tok;
  }
  
  char *str = malloc(t->pos - start);
  strncpy(str, &t->input[start], t->pos - start);
  str[t->pos - start] = '\0';

  tok.type = TOK_IDENT;
  tok.val.ident = str;
  return tok;
}
