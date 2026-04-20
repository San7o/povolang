// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"

#include <stdio.h>

typedef struct parser {
  token_stream_t *ts;
} parser_t;

token_t peek(parser_t *p)
{
  return token_stream_peek(p->ts);
}

void advance(parser_t *p)
{
  token_stream_advance(p->ts);
}

void parser_init(parser_t *p, token_stream_t *ts)
{
  if (!p) return;

  p->ts = ts;
}

void parser_init_from_input(parser_t *p, char *input)
{
  if (!p || !input) return;

  token_stream_init_from_input(p->ts, input);
}

// STMT ::= IF_STMT | WHILE_STMT | FOR_STMT| BLOCK | EXPR `;`
ast_node_t *parse_stmt(parser_t *p)
{
  (void) p;
  // TODO
  return NULL;
}

// FUNCTION ::= fn IDENT ( ( IDENT , )* ) BLOCK?
ast_node_t *parse_function(parser_t *p)
{
  (void) p;
  // TODO
  return NULL;
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
