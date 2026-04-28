// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "../lang.h"
#include "micro-tests.h"

#include <string.h>

TEST(parser_tests, factor_literal_int)
{
  parser_t p;
  parser_init_from_input(&p, "123");

  ast_node_t *node = parse_factor(&p);
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_LITERAL);
  ASSERT(node->val.literal.type == LITERAL_INT);
  ASSERT(node->val.literal.val.integer == 123);
  ast_free_node(node);
  
  TEST_SUCCESS;
}

TEST(parser_tests, factor_literal_float)
{
  parser_t p;
  parser_init_from_input(&p, "123.123");

  ast_node_t *node = parse_factor(&p);
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_LITERAL);
  ASSERT(node->val.literal.type == LITERAL_FLOAT);
  ASSERT(node->val.literal.val.floating - 123.123 < 1e-4
         && node->val.literal.val.floating - 123.123 > -1e-4);
  ast_free_node(node);
  
  TEST_SUCCESS;
}

TEST(parser_tests, factor_literal_bool_true)
{
  parser_t p;
  parser_init_from_input(&p, "true");

  ast_node_t *node = parse_factor(&p);
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_LITERAL);
  ASSERT(node->val.literal.type == LITERAL_BOOL);
  ASSERT(node->val.literal.val.boolean == true);
  ast_free_node(node);
  
  TEST_SUCCESS;
}

TEST(parser_tests, factor_literal_bool_false)
{
  parser_t p;
  parser_init_from_input(&p, "false");

  ast_node_t *node = parse_factor(&p);
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_LITERAL);
  ASSERT(node->val.literal.type == LITERAL_BOOL);
  ASSERT(node->val.literal.val.boolean == false);
  ast_free_node(node);
  
  TEST_SUCCESS;
}

TEST(parser_tests, factor_literal_string)
{
  parser_t p;
  parser_init_from_input(&p, "\"Hello\"");

  ast_node_t *node = parse_factor(&p);
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_LITERAL);
  ASSERT(node->val.literal.type == LITERAL_STRING);
  ASSERT(strcmp(node->val.literal.val.string, "Hello") == 0);
  ast_free_node(node);
  
  TEST_SUCCESS;
}

TEST(parser_tests, factor_fn_call)
{
  parser_t p;
  parser_init_from_input(&p, "printf(stdout, \"Hello, World: %s\", 123)");

  ast_node_t *node = parse_factor(&p);
  
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_FN_CALL);
  ASSERT(strcmp(node->val.fn_call.fn_name, "fprintf"));
  ASSERT(node->val.fn_call.count == 3);

  ASSERT(node->val.fn_call.args[0] != NULL);
  ASSERT(node->val.fn_call.args[0]->type == NODE_VARIABLE);
  ASSERT(strcmp(node->val.fn_call.args[0]->val.variable_name, "stdout") == 0);

  ASSERT(node->val.fn_call.args[1] != NULL);
  ASSERT(node->val.fn_call.args[1]->type == NODE_LITERAL);
  ASSERT(node->val.fn_call.args[1]->val.literal.type == LITERAL_STRING);
  ASSERT(strcmp(node->val.fn_call.args[1]->val.literal.val.string,
                "Hello, World: %s") == 0);
  
  ASSERT(node->val.fn_call.args[2] != NULL);
  ASSERT(node->val.fn_call.args[2]->type == NODE_LITERAL);
  ASSERT(node->val.fn_call.args[2]->val.literal.type == LITERAL_INT);
  ASSERT(node->val.fn_call.args[2]->val.literal.val.integer == 123);
  
  ast_free_node(node);
  
  TEST_SUCCESS;
}

TEST(parser_tests, factor_expr)
{
  parser_t p;
  parser_init_from_input(&p, "( 123 + 456 )");

  ast_node_t *node = parse_factor(&p);
  
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_BINARY_OP);
  ASSERT(node->val.binary.op == OP_ADD);

  ASSERT(node->val.binary.left != NULL);
  ASSERT(node->val.binary.left->type == NODE_LITERAL);
  ASSERT(node->val.binary.left->val.literal.type == LITERAL_INT);
  ASSERT(node->val.binary.left->val.literal.val.integer == 123);
  
  ASSERT(node->val.binary.right != NULL);
  ASSERT(node->val.binary.right->type == NODE_LITERAL);
  ASSERT(node->val.binary.right->val.literal.type == LITERAL_INT);
  ASSERT(node->val.binary.right->val.literal.val.integer == 456);
  
  ast_free_node(node);
  
  TEST_SUCCESS;
}
