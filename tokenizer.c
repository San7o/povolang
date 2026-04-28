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
  while (isalnum(t->input[t->pos]) && t->pos <= t->size) t->pos++;

  if (t->pos == t->size + 1 || t->pos == start) return tok;
  
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

void free_tok(token_t *tok)
{
  if (!tok) return;

  switch (tok->type)
  {
  case TOK_IDENT: free(tok->val.ident); tok->val.ident = NULL; return;
  default: return;
  }
}

char *token_string(token_type_t tok)
{
  switch(tok)
  {
  case TOK_EOF:          return "EOF";
  case TOK_INT:          return "INT";              // 123
  case TOK_FLOAT:        return "FLOAT";            // 123.123
  case TOK_IDENT:        return "IDENT";            // hello
  case TOK_STRING:       return "STRING";           // "hello"
  case TOK_TRUE:         return "TRUE";
  case TOK_FALSE:        return "FALSE";
  case TOK_IF:           return "IF";
  case TOK_ELSE:         return "ELSE";
  case TOK_WHILE:        return "WHILE";
  case TOK_FOR:          return "FOR";
  case TOK_FN:           return "FN";
  case TOK_RETURN:       return "RETURN";
  case TOK_OPEN_PAREN:   return "OPEN_PAREN";       // (
  case TOK_CLOSE_PAREN:  return "CLOSE_PAREN";      // )
  case TOK_OPEN_SQUARE:  return "OPEN_SQUARE";      // [
  case TOK_CLOSE_SQUARE: return "CLOSE_SQUARE";     // ]
  case TOK_OPEN_CURLY:   return "OPEN_CURLY";       // {
  case TOK_CLOSE_CURLY:  return "CLOSE_CURLY";      // }
  case TOK_SEMICOLON:    return "SEMICOLON";        // ;
  case TOK_COMMA:        return "COMMA";            // ,
  case TOK_PLUS:         return "PLUS";             // +
  case TOK_MINUS:        return "MINUS";            // -
  case TOK_STAR:         return "STAR";             // *
  case TOK_DIV:          return "DIV";              // /
  case TOK_NOT:          return "NOT";              // !
  case TOK_AND:          return "AND";              // &&
  case TOK_OR:           return "OR";               // ||
  case TOK_ASSIGN:       return "ASSIGN";           // =
  case TOK_EQUAL:        return "EQUAL";            // ==
  case TOK_NOT_EQUAL:    return "NOT_EQUAL";        // !=
  case TOK_LT:           return "LESS_THAN";        // <
  case TOK_GT:           return "GREATER_THAN";     // >
  default: return "TOKEN_UNKNOWN";
  }
}
