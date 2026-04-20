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

typedef enum token_type {
  TOK_EOF = 0,

  // Literals
  TOK_INT,              // 123
  TOK_FLOAT,            // 123.123
  TOK_IDENT,            // hello
  TOK_STRING,           // "hello"
  // Booleans
  TOK_TRUE,
  TOK_FALSE,

  // Keywords
  TOK_IF,
  TOK_ELSE,
  TOK_WHILE,
  TOK_FOR,
  TOK_FN,
  TOK_RETURN,

  // Symbols
  TOK_OPEN_PAREN,       // (
  TOK_CLOSE_PAREN,      // )
  TOK_OPEN_SQUARE,      // [
  TOK_CLOSE_SQUARE,     // ]
  TOK_OPEN_CURLY,       // {
  TOK_CLOSE_CURLY,      // }
  TOK_SEMICOLON,        // ;
  TOK_COMMA,            // ,
  
  // Math ops and logic
  TOK_PLUS,             // +
  TOK_MINUS,            // -
  TOK_STAR,             // *
  TOK_DIV,              // /
  TOK_NOT,              // !
  TOK_AND,              // &&
  TOK_OR,               // ||
  TOK_ASSIGN,           // =
  TOK_EQUAL,            // ==
  TOK_NOT_EQUAL,        // !=
  TOK_LT,               // <
  TOK_GT,               // >
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
  tok.value.ident = str;
  return tok;
}

// Dynamic array of tokens
typedef struct token_stream {
  token_t *tokens;
  size_t   capacity;
  size_t   size;
  size_t   pos;
} token_stream_t;

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
    memcpy(new_vec, ts->tokens, ts->capacity * sizeof(token_t));
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
    case TOK_INT:           printf("%d\n", tok.value.integer);  break;
    case TOK_FLOAT:         printf("%f\n", tok.value.floating); break;
    case TOK_IDENT:         printf("%s\n", tok.value.ident);   free(tok.value.ident); break;
    case TOK_STRING:        printf("\"%s\"\n", tok.value.str); free(tok.value.str);   break;
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
// IF_STMT    ::= `if` `(` EXPR `)` BLOCK ( `else` BLOCK )?
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

// TODO

//
// Code Generation
//

// TODO

int main(void)
{
  char *input = 
    "int main(void) { "
    "  // comment!\n"
    "  printf(\"Hello, world! %f\\n\", 69.420);"
    "  if (x < 10) return 0;"
    "  else return 1;"
    "}";
  token_stream_t ts;
  token_stream_init_from_input(&ts, input);
  token_stream_dump(&ts);
  return 0;
}
