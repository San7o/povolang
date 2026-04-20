// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef LANG_H
#define LANG_H

#include <stdbool.h>
#include <stddef.h>

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
  } val;
} token_t;

typedef struct tokenizer {
  const char *input;
  int size;
  int pos;
} tokenizer_t;

tokenizer_t init_tok(char* input);
token_t next_tok(tokenizer_t *t);

//
// Token stream
// ------------
//

typedef struct token_stream {
  token_t *tokens;
  size_t   capacity;
  size_t   size;
  size_t   pos;
} token_stream_t;

void token_stream_init(token_stream_t *ts);
void token_stream_init_from_input(token_stream_t *ts, char *input);
void token_stream_add(token_stream_t *ts, token_t tok);
token_t token_stream_peek(token_stream_t *ts);
void token_stream_advance(token_stream_t *ts);
// Consumes the stream and prints it to stdout
void token_stream_dump(token_stream_t *ts);

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
  NODE_TYPE_BINARY_OP,   // +, -, *, /
  NODE_TYPE_LITERAL,     // 1, 2.5, "hello"
  NODE_TYPE_VARIABLE,    // x, y
  NODE_TYPE_FN_CALL,     // print(x)

  // Statements / Control flow
  NODE_TYPE_IF,
  NODE_TYPE_FOR,
  NODE_TYPE_WHILE,
  NODE_TYPE_ASSIGNMENT,
  NODE_TYPE_BLOCK,
  NODE_TYPE_FN_DECL,
  NODE_TYPE_FN_IMPL,
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
      ast_node_t *left;
      ast_node_t *right;
    } binary;
    
    struct {
      ast_node_t *cond;
      ast_node_t *then_block;
      ast_node_t *else_block;
    } if_stmt;

    struct {
      ast_node_t *init;
      ast_node_t *cond;
      ast_node_t *step;
      ast_node_t *body;
    } for_stmt;
    
    struct {
      ast_node_t *cond;
      ast_node_t *body;
    } while_stmt;
    
    struct {
      ast_node_t **children;
      size_t capacity;
      size_t count;
    } block;

    struct {
      char       *name;
      ast_node_t *value;
    } assignment;

    struct {
      char        *fn_name;
      ast_node_t **args;
      size_t       capacity;
      size_t       count;
    } fn_call;

    struct {
      char *name;
      char **params;
      size_t capacity;
      size_t param_count;
    } fn_decl;

    struct {
      char *name;
      char **params;
      size_t params_capacity;
      size_t params_count;
      ast_node_t *body;
    } fn_impl;
    
    literal_t literal;
    char*   variable_name;
    
  } val;
};

ast_node_t *ast_new_node(node_type_t type);
void ast_free_node(ast_node_t *node);
void ast_add_child_to_block(ast_node_t *block, ast_node_t *child);

//
// Parser
// ------
//

// TODO

//
// Code generation
// ---------------
//

// TODO

#endif // LANG_H
