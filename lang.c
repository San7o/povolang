// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//
// Tokenizer
// ---------
//

// C-like tokens
typedef enum token_type {
  TOK_NONE = 0,
  TOK_INT,              // 123
  TOK_FLOAT,            // 123.123
  TOK_IDENT,            // hello
  TOK_STRING,           // "hello"
  TOK_OPEN_PAREN,       // (
  TOK_CLOSE_PAREN,      // )
  TOK_OPEN_SQUARE,      // [
  TOK_CLOSE_SQUARE,     // ]
  TOK_OPEN_CURLY,       // {
  TOK_CLOSE_CURLY,      // }
  TOK_COLON,            // ;
  TOK_COMMA,            // ,
  
  // Math ops
  TOK_PLUS,             // +
  TOK_MINUS,            // -
  TOK_STAR,             // *
  TOK_DIV,              // /

  // Logical ops
  TOK_AND,              // &&
  TOK_OR,               // ||
} token_type_t;

typedef struct token {
  token_type_t type;
  union {
    int   integer;
    float floating;
    char* ident;
    char* str;
  } value;
} token_t;

typedef struct tokenizer {
  const char *input;
  int size;
  int pos;

  int row;
  int col;
} tokenizer_t;

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
    .type    = TOK_NONE,
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
  case ';': tok.type = TOK_COLON;        t->pos++; return tok;
  case ',': tok.type = TOK_COMMA;        t->pos++; return tok;
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
    tok.value.str = str;
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
      tok.value.floating = fnum;
      return tok;
    }
    else
    {
      tok.type = TOK_INT;
      tok.value.integer = num;
      return tok;
    }
  }

  // Identifier
  int start = t->pos;
  while (isalnum(t->input[t->pos]) && t->pos < t->size) t->pos++;

  if (t->pos == t->size || t->pos == start) return tok;

  char *str = malloc(t->pos - start);
  strncpy(str, &t->input[start], t->pos - start);
  str[t->pos - start] = '\0';
      
  tok.type = TOK_IDENT;
  tok.value.ident = str;
  return tok;
}

void dump_tok(tokenizer_t *t)
{
  token_t tok = next_tok(t);
  while (tok.type != TOK_NONE)
  {
    switch (tok.type)
    {
    case TOK_INT:           printf("%d\n", tok.value.integer);  break;
    case TOK_FLOAT:         printf("%f\n", tok.value.floating); break;
    case TOK_IDENT:         printf("%s\n", tok.value.ident);   free(tok.value.ident);  break;
    case TOK_STRING:        printf("\"%s\"\n", tok.value.str); free(tok.value.str); break;
    case TOK_OPEN_PAREN:    printf("(\n");  break;
    case TOK_CLOSE_PAREN:   printf(")\n");  break;
    case TOK_OPEN_SQUARE:   printf("[\n");  break;
    case TOK_CLOSE_SQUARE:  printf("]\n");  break;
    case TOK_OPEN_CURLY:    printf("{\n");  break;
    case TOK_CLOSE_CURLY:   printf("}\n");  break;
    case TOK_COLON:         printf(";\n");  break;
    case TOK_COMMA:         printf(",\n");  break;
    case TOK_PLUS:          printf("+\n");  break;
    case TOK_MINUS:         printf("-\n");  break;
    case TOK_STAR:          printf("*\n");  break;
    case TOK_DIV:           printf("/\n");  break;
    case TOK_AND:           printf("&&\n"); break;
    case TOK_OR:            printf("||\n"); break;
    default:                printf("TOK_UNKNOWN\n"); break;
    }

    tok = next_tok(t);
  }
}

//
// Grammar
// -------
//
// Terminals are enclosed in ``. Whitespaces are ignored.
//
// Starting symbol:
//
// PROGRAM    ::= ( FN_IMPL | STMT )*
//
// Literal:
//
// NUM        ::= [0-9]
// INTEGER    ::= NUM
// FLOATING   ::= NUM.NUM
// BOOLEAN    ::= `true` | `false`
// STRING     ::= `"` ASCII* `"`
// IDENT      ::= [a-zA-Z][a-zA-Z0-9]*
// LITERAL    ::= INTEGER | FLOATING | BOOLEAN | STRING
//
// EXPR       ::= ASSIGNMENT
//              | ARITH_EXPR
// ASSIGNMENT ::= IDENT `=` EXPR
//
// Math ops:
// 
// ARITH_EXPR   ::= TERM ( (`+` | `-` ) TERM )*
// TERM         ::= FACTOR ( (`*` | `/`) FACTOR )*
// FACTOR       ::= LITERAL | IDENT | FN_CALL | `(` EXPR `)`
//
// Note that these math operations are simply the non-left-recursive
// version of the following grammar, which respects operator
// precedence:
//
//         ARITH_EXPR   ::= ARITH_EXPR `+` TERM
//                        | ARITH_EXPR `-` TERM
//         TERM         ::= TERM `*` FACTOR
//                          TERM `/` FACTOR
// 
//
// Control flow statements:
//
// STMT       ::= IF_STMT | WHILE_STMT | FOR_STMT| BLOCK | EXPR `;`
// BLOCK      ::= `{` STMT* `}`
//
// The `else` part of an if statement is optional, and the parser will
// always match the closes if. This avoids ambiguity:
//
// IF_STMT    ::= `if` `(` EXPR `)` `then` BLOCK ( `else` BLOCK )?
//
// FOR_STMT   ::= `for` `(` ASSIGNMENT `;` EXPR `;` EXPR `)` BLOCK
// WHILE_STMT ::= `while` `(` EXPR `)` BLOCK
// FN_CALL    ::= IDENT `(` ( EXPR `,` )* `)`
// FN_DECL    ::= fn IDENT ( ( IDENT , )* )
// FN_IMPL    ::= fn IDENT ( ( IDENT , )* ) BLOCK
//

//
// AST
// ---
//

typedef enum literal_type {
  LITERAL_TYPE_VOID = 0,
  LITERAL_TYPE_INT,
  LITERAL_TYPE_FLOAT,
  LITERAL_TYPE_BOOL,
  LITERAL_TYPE_STRING,
} value_type_t;

typedef struct literal {
  value_type_t type;
  union {
    int       integer;
    float     floating;
    bool      boolean;
    char     *string;
  } val;
} literal_t;

typedef enum node_type {
  // Expressions
  NODE_BINARY_OP,   // +, -, *, /
  NODE_LITERAL,     // 1, 2.5, "hello"
  NODE_VARIABLE,    // x, y
  NODE_FN_CALL,     // print(x)

  // Statements / Control flow
  NODE_IF,
  NODE_FOR,
  NODE_WHILE,
  NODE_ASSIGNMENT,
  NODE_BLOCK,
  NODE_FN_DECL,
  NODE_FN_IMPL,
} node_type_t;

typedef enum op_type {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_EQ,    // ==
  OP_NEQ,   // !=
  OP_GT,    // >
  OP_LT,    // <
} op_type_t;

typedef struct ast_node ast_node_t;

struct ast_node {
  node_type_t type;
  union {

    struct {
      op_type_t op;
      struct ast_node *left;
      struct ast_node *right;
    } binary;
    
    struct {
      struct ast_node *cond;
      struct ast_node *then_block;
      struct ast_node *else_block;
    } if_stmt;

    struct {
      struct ast_node *init;
      struct ast_node *cond;
      struct ast_node *step;
      struct ast_node *body;
    } for_stmt;
    
    struct {
      struct ast_node *cond;
      struct ast_node *body;
    } while_stmt;
    
    struct {
      struct ast_node **children;
      size_t count;
    } block;

    struct {
      char *name;
      struct ast_node *value;
    } assignment;

    struct {
      char        *fn_name;
      ast_node_t **args;
      size_t       count;
    } fn_call;

    struct {
      char *name;
      char **params;
      size_t param_count;
    } fn_decl;

    struct {
      char *name;
      char **params;
      size_t param_count;
      struct ast_node *body;
    } fn_impl;
    
    literal_t literal;
    char*   variable_name;
    
  } val;
};



//
// Code Generation
//

// TODO

int main(void)
{
  char *input = "int main(void) { // comment!\n printf(\"Hello, world! %f\\n\", 69.420); }";
  tokenizer_t t = init_tok(input);
  dump_tok(&t);
  return 0;
}
