// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"

#include <stdio.h>
#include <stdlib.h>

token_t peek(parser_t *p)
{
  return token_stream_peek(p->ts);
}

void advance(parser_t *p)
{
  token_stream_advance(p->ts);
}

void expect(parser_t *p, token_type_t token)
{
  if (peek(p).type != token)
  {
    size_t start = p->ts->pos;
    if (start - 5 > 0)
      start -= 5;
    for (size_t i = start; i < p->ts->pos; ++i)
    {
      token_dump(p->ts->tokens[i]);
      printf(" ");
    }
    printf("\n");
      
    printf("Error: expected %s, got %s\n",
            token_string(token), token_string(peek(p).type));
    fflush(stdout);
    exit(1);
  }
}

void parser_init(parser_t *p, token_stream_t *ts)
{
  if (!p) return;

  p->ts = ts;
}

void parser_init_from_input(parser_t *p, char *input)
{
  if (!p || !input) return;

  p->ts = malloc(sizeof(token_stream_t));
  token_stream_init_from_input(p->ts, input);
}

// Parse grammar

// FACTOR ::= LITERAL | IDENT ( `(` ARGS `)` )? | `(` EXPR `)`
ast_node_t *parse_factor(parser_t *p)
{
  ast_node_t *node = NULL;

  switch (peek(p).type)
  {
  case TOK_INT:
  {
    token_t tok = peek(p);
    advance(p);
    node = ast_new_node(NODE_LITERAL);
    node->val.literal = (literal_t) {
      .type = LITERAL_INT,
      .val.integer = tok.val.integer,
    };
    break;
  }
  case TOK_FLOAT:
  {
    token_t tok = peek(p);
    advance(p);
    node = ast_new_node(NODE_LITERAL);
    node->val.literal = (literal_t) {
      .type = LITERAL_FLOAT,
      .val.floating = tok.val.floating,
    };
    break;
  }
  case TOK_STRING:
  {
    token_t tok = peek(p);
    advance(p);
    node = ast_new_node(NODE_LITERAL);
    node->val.literal = (literal_t) {
      .type = LITERAL_STRING,
      .val.string = tok.val.str,
    };
    break;
  }
  case TOK_TRUE:
  {
    advance(p);
    node = ast_new_node(NODE_LITERAL);
    node->val.literal = (literal_t) {
      .type = LITERAL_BOOL,
      .val.boolean = true,
    };
    break;
  }
  case TOK_FALSE:
  {
    advance(p);
    node = ast_new_node(NODE_LITERAL);
    node->val.literal = (literal_t) {
      .type = LITERAL_BOOL,
      .val.boolean = false,
    };
    break;
  }
  case TOK_IDENT:
  {
    token_t name = peek(p);
    advance(p);

    // fn call
    if (peek(p).type == TOK_OPEN_PAREN)
    {
      node = ast_new_node(NODE_FN_CALL);
      node->val.fn_call.fn_name = name.val.ident;

      if (peek(p).type != TOK_CLOSE_PAREN)
      {
        do {
          advance(p); // Advance comma or open paren

          ast_node_t *expr = parse_expr(p);
          if (!expr)
          {
            fprintf(stderr, "Error parsing expression in function call\n");
            break;
          }

          ast_add_arg_to_function(node, expr);
    
        } while (peek(p).type == TOK_COMMA);
      }

      expect(p, TOK_CLOSE_PAREN);
      advance(p);
    }
    else
    {
      node = ast_new_node(NODE_VARIABLE);
      node->val.variable_name = name.val.ident;
    }
    
    break;
  }
  case TOK_OPEN_PAREN:
    advance(p);
    node = parse_expr(p);
    expect(p, TOK_CLOSE_PAREN);
    advance(p);
    break;
  default:
    fprintf(stderr, "Error unexpected token %s\n",
            token_string(peek(p).type));
    return NULL;
  }
  
  return node;
}

// TERM ::= FACTOR ( (`*` | `/`) FACTOR )*
ast_node_t *parse_term(parser_t *p)
{
  ast_node_t *left = parse_factor(p);

  while (peek(p).type == TOK_STAR || peek(p).type == TOK_DIV)
  {
    token_t op = peek(p);
    advance(p);

    ast_node_t *right = parse_factor(p);

    ast_node_t *new_node = ast_new_node(NODE_BINARY_OP);
    if (op.type == TOK_STAR)
      new_node->val.binary.op = OP_MUL;
    else
      new_node->val.binary.op = OP_DIV;      
    new_node->val.binary.left = left;
    new_node->val.binary.right = right;

    left = new_node;
  }

  return left;
}

// ARITH_EXPR  ::= TERM ( (`+` | `-` ) TERM )*
ast_node_t *parse_arith_expr(parser_t *p)
{
  ast_node_t *left = parse_term(p);

  while (peek(p).type == TOK_PLUS || peek(p).type == TOK_MINUS)
  {
    token_t op = peek(p);
    advance(p);

    ast_node_t *right = parse_factor(p);

    ast_node_t *new_node = ast_new_node(NODE_BINARY_OP);
    if (op.type == TOK_PLUS)
      new_node->val.binary.op = OP_ADD;
    else
      new_node->val.binary.op = OP_SUB;      
    new_node->val.binary.left = left;
    new_node->val.binary.right = right;

    left = new_node;
  }

  return left;
}

// COMPARISON_OP   ::= ( `==` | `!=` | `<` | `>` )
// COMPARISON_EXPR ::= ARITH_EXPR ( COMPARISON_OP ARITH_EXPR )*
ast_node_t *parse_comparison(parser_t *p)
{
  ast_node_t *left = parse_arith_expr(p);

  // Comparison
  while (peek(p).type == TOK_EQUAL || peek(p).type == TOK_NOT_EQUAL
         || peek(p).type == TOK_LT || peek(p).type == TOK_GT)
  {
    token_t tok = peek(p);
    switch (tok.type)
    {
    case TOK_EQUAL:
    {
      advance(p);
      ast_node_t *right = parse_arith_expr(p);
        
      ast_node_t *node = ast_new_node(NODE_BINARY_OP);
      node->val.binary.op    = OP_EQ;
      node->val.binary.left  = left;
      node->val.binary.right = right;
        
      return node;
    }
    case TOK_NOT_EQUAL:
    {
      advance(p);
      ast_node_t *right = parse_arith_expr(p);
        
      ast_node_t *node = ast_new_node(NODE_BINARY_OP);
      node->val.binary.op    = OP_NEQ;
      node->val.binary.left  = left;
      node->val.binary.right = right;
        
      return node;
    }
    case TOK_LT:
    {
      advance(p);
      ast_node_t *right = parse_arith_expr(p);
        
      ast_node_t *node = ast_new_node(NODE_BINARY_OP);
      node->val.binary.op    = OP_LT;
      node->val.binary.left  = left;
      node->val.binary.right = right;
        
      return node;
    }
    case TOK_GT:
    {
      advance(p);
      ast_node_t *right = parse_arith_expr(p);
        
      ast_node_t *node = ast_new_node(NODE_BINARY_OP);
      node->val.binary.op    = OP_GT;
      node->val.binary.left  = left;
      node->val.binary.right = right;
        
      return node;
    }
    default:
      fprintf(stderr, "Error unknown operator in expression\n");
      return NULL;
    }
  }

  return left;
}

// EXPR ::= ARITH_EXPR ( `=` EXPR )?
ast_node_t *parse_expr(parser_t *p)
{
  ast_node_t *left = parse_comparison(p);
  if (!left) return NULL;

  if (peek(p).type == TOK_ASSIGN)
  {
    advance(p);

    ast_node_t *right = parse_expr(p);
    if (!right) return NULL;

    ast_node_t *assign_node = ast_new_node(NODE_ASSIGNMENT);
    assign_node->val.assignment.left  = left;
    assign_node->val.assignment.right = right;
    return assign_node;
  }
  
  return left;
}

// FOR_STMT ::= `for` `(` EXPR `;` EXPR `;` EXPR `)` BLOCK
ast_node_t *parse_for_stmt(parser_t *p)
{
  ast_node_t *node = NULL;
  ast_node_t *init = NULL;
  ast_node_t *cond = NULL;
  ast_node_t *step = NULL;
  ast_node_t *body = NULL;

  expect(p, TOK_FOR);
  advance(p);
  expect(p, TOK_OPEN_PAREN);
  advance(p);

  init = parse_expr(p);

  expect(p, TOK_SEMICOLON);
  advance(p);

  cond = parse_expr(p);

  expect(p, TOK_SEMICOLON);
  advance(p);

  step = parse_expr(p);
  advance(p);

  expect(p, TOK_CLOSE_PAREN);
  advance(p);

  body = parse_block(p);

  node = ast_new_node(NODE_FOR);
  node->val.for_stmt.init = init;
  node->val.for_stmt.cond = cond;
  node->val.for_stmt.step = step;
  node->val.for_stmt.body = body;
  
  return node;
}

// WHILE_STMT ::= `while` `(` EXPR `)` BLOCK`
ast_node_t *parse_while_stmt(parser_t *p)
{
  ast_node_t *node = NULL;
  ast_node_t *cond = NULL;
  ast_node_t *body = NULL;

  expect(p, TOK_WHILE);
  advance(p);
  expect(p, TOK_OPEN_PAREN);
  advance(p);

  cond = parse_expr(p);

  expect(p, TOK_CLOSE_PAREN);
  advance(p);

  body = parse_block(p);

  node = ast_new_node(NODE_WHILE);
  node->val.while_stmt.cond = cond;
  node->val.while_stmt.body = body;
  
  return node;
}

// IF_STMT ::= `if` `(` EXPR `)` BLOCK ( `else` BLOCK )?
ast_node_t *parse_if_stmt(parser_t *p)
{
  ast_node_t *node = NULL;
  ast_node_t *cond = NULL;
  ast_node_t *then_block = NULL;
  ast_node_t *else_block = NULL;

  expect(p, TOK_IF);
  advance(p);
  expect(p, TOK_OPEN_PAREN);
  advance(p);

  cond = parse_expr(p);
  if (!cond)
  {
    fprintf(stderr, "Error: parsing if condition\n");
    return NULL;
  }

  expect(p, TOK_CLOSE_PAREN);
  advance(p);

  then_block = parse_block(p);
  if (!then_block)
  {
    fprintf(stderr, "Error: parsing if block\n");
    ast_free_node(cond);
    return NULL;
  }

  if (peek(p).type == TOK_ELSE)
  {
    advance(p);
    else_block = parse_block(p);
  }

  node = ast_new_node(NODE_IF);
  node->val.if_stmt.cond = cond;
  node->val.if_stmt.then_block = then_block;
  node->val.if_stmt.else_block = else_block;
  
  return node;
}

// BLOCK ::= `{` STMT* `}`
ast_node_t *parse_block(parser_t *p)
{
  ast_node_t *node = NULL;
  
  expect(p, TOK_OPEN_CURLY);
  advance(p);

  node = ast_new_node(NODE_BLOCK);
  
  while (peek(p).type != TOK_CLOSE_CURLY)
  {
    ast_node_t *stmt = parse_stmt(p);
    if (!stmt)
    {
      fprintf(stderr, "Error parsing statement in block\n");
      return NULL;
    }

    ast_add_child_to_block(node, stmt);
  }
  
  expect(p, TOK_CLOSE_CURLY);
  advance(p);

  return node;
}

// STMT ::= IF_STMT | WHILE_STMT | FOR_STMT| BLOCK | EXPR `;`
ast_node_t *parse_stmt(parser_t *p)
{
  ast_node_t *node = NULL;
  
  switch(peek(p).type)
  {
  case TOK_IF:
    node = parse_if_stmt(p);
    break;
  case TOK_WHILE:
    node = parse_while_stmt(p);
    break;
  case TOK_FOR:
    node = parse_for_stmt(p);
    break;
  case TOK_OPEN_CURLY:
    node = parse_block(p);
    break;
  default:
    node = parse_expr(p);
    expect(p, TOK_SEMICOLON);
    advance(p);
    break;
  }
  return node;
}

#define args_append(xs, x)                                                           \
    do {                                                                             \
        if ((xs)->count >= (xs)->capacity) {                                         \
            if ((xs)->capacity == 0) (xs)->capacity = 2;                             \
            else (xs)->capacity *= 2;                                                \
            (xs)->args = realloc((xs)->args, (xs)->capacity*sizeof(*(xs)->args));    \
        }                                                                            \
                                                                                     \
        (xs)->args[(xs)->count++] = (x);                                             \
    } while (0)

typedef struct args {
  char  **args;
  size_t  count;
  size_t  capacity;
} args_t;

// FN_DECL    ::= fn IDENT ( ( IDENT , )* )
// FN_IMPL    ::= fn IDENT ( ( IDENT , )* ) BLOCK
ast_node_t *parse_function(parser_t *p)
{
  char *fn_name = NULL;
  expect(p, TOK_FN);
  advance(p);

  expect(p, TOK_IDENT);
  fn_name = peek(p).val.str;
  advance(p);

  expect(p, TOK_OPEN_PAREN);
  advance(p);

  args_t args = {0};

  while (peek(p).type != TOK_CLOSE_PAREN)
  {
    expect(p, TOK_IDENT);
    char* arg = peek(p).val.ident;
    advance(p);

    args_append(&args, arg);

    
    if (peek(p).type != TOK_CLOSE_PAREN && peek(p).type != TOK_COMMA)
    {
      fprintf(stderr, "Error: expected `)` or `,`, but got %d",
              peek(p).type);
      return NULL;
    }
  }

  expect(p, TOK_CLOSE_PAREN);
  advance(p);

  if (peek(p).type == TOK_OPEN_CURLY)
  {
    // Impl
    ast_node_t *block = parse_block(p);

    ast_node_t *node = ast_new_node(NODE_FN_IMPL);
    node->val.fn_impl.name = fn_name;
    node->val.fn_impl.body = block;
    node->val.fn_impl.params = args.args;
    node->val.fn_impl.count = args.count;
    node->val.fn_impl.capacity = args.count;

    return node;
  }

  ast_node_t *node = ast_new_node(NODE_FN_DECL);  
  node->val.fn_impl.name = fn_name;
  node->val.fn_decl.params = args.args;
  node->val.fn_decl.count = args.count;
  node->val.fn_decl.capacity = args.count;

  return node;
}

// PROGRAM  ::= ( FN_DECL | FN_IMPL | STMT )*
ast_node_t *parse_program(parser_t *p)
{
  ast_node_t *root = ast_new_node(NODE_BLOCK);
  
  while (peek(p).type != TOK_EOF)
  {
    ast_node_t *node = NULL;

    if (peek(p).type == TOK_FN) node = parse_function(p);
    else node = parse_stmt(p);

    if (!node)
    {
      fprintf(stderr, "Error: Unexpected token %d\n", peek(p).type);
      ast_free_node(root);
      return NULL;
    }

    ast_add_child_to_block(root, node);
  }

  return root;
}
