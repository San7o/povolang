// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"

#include <stdlib.h>
#include <string.h>

ast_node_t *ast_new_node(node_type_t type)
{
  ast_node_t *node = malloc(sizeof(ast_node_t));
  memset(node, 0, sizeof(ast_node_t));
  node->type = type;
  return node;
}

void ast_free_node(ast_node_t *node)
{
  if (!node) return;

  switch(node->type)
  {
  case NODE_LITERAL:
  {
    if (node->val.literal.type == LITERAL_STRING)
      if (node->val.literal.val.string)
      {
        free(node->val.literal.val.string);
        node->val.literal.val.string = NULL;
      }
    
    break;
  }
  case NODE_VARIABLE:
  {
    if (node->val.variable_name)
    {
      free(node->val.variable_name);
      node->val.variable_name = NULL;
    }
    break;
  }
  case NODE_FN_CALL:
  {
    if (node->val.fn_call.fn_name)
    {
      free(node->val.fn_call.fn_name);
      node->val.fn_call.fn_name = NULL;
    }
    if (node->val.fn_call.args)
    {
      for (size_t i = 0; i < node->val.fn_call.count; ++i)
      {
        ast_free_node(node->val.fn_call.args[i]);
        free(node->val.fn_call.args[i]);
        node->val.fn_call.args[i] = NULL;
      }
      free(node->val.fn_call.args);
      node->val.fn_call.args = NULL;
      node->val.fn_call.capacity = 0;
      node->val.fn_call.count    = 0;
    }
    break;
  }
  case NODE_IF:
  {
    if (node->val.if_stmt.cond)
    {
      ast_free_node(node->val.if_stmt.cond);
      free(node->val.if_stmt.cond);
      node->val.if_stmt.cond = NULL;
    }
    if (node->val.if_stmt.then_block)
    {
      ast_free_node(node->val.if_stmt.then_block);
      free(node->val.if_stmt.then_block);
      node->val.if_stmt.then_block = NULL;
    }
    if (node->val.if_stmt.else_block)
    {
      ast_free_node(node->val.if_stmt.else_block);
      free(node->val.if_stmt.else_block);
      node->val.if_stmt.else_block = NULL;
    }
    break;
  }
  case NODE_FOR:
  {
    if (node->val.for_stmt.init)
    {
      ast_free_node(node->val.for_stmt.init);
      free(node->val.for_stmt.init);
      node->val.for_stmt.init = NULL;
    }
    if (node->val.for_stmt.cond)
    {
      ast_free_node(node->val.for_stmt.cond);
      free(node->val.for_stmt.cond);
      node->val.for_stmt.cond = NULL;
    }
    if (node->val.for_stmt.step)
    {
      ast_free_node(node->val.for_stmt.step);
      free(node->val.for_stmt.step);
      node->val.for_stmt.step = NULL;
    }
    if (node->val.for_stmt.body)
    {
      ast_free_node(node->val.for_stmt.body);
      free(node->val.for_stmt.body);
      node->val.for_stmt.body = NULL;
    }
    break;
  }
  case NODE_WHILE:
  {
    if (node->val.while_stmt.cond)
    {
      ast_free_node(node->val.while_stmt.cond);
      free(node->val.while_stmt.cond);
      node->val.while_stmt.cond = NULL;
    }
    if (node->val.while_stmt.body)
    {
      ast_free_node(node->val.while_stmt.body);
      free(node->val.while_stmt.body);
      node->val.while_stmt.body = NULL;
    }
    break;
  }
  case NODE_BLOCK:
  {
    if (node->val.block.children)
    {
      for (size_t i = 0; i < node->val.block.count; ++i)
      {
        ast_free_node(node->val.block.children[i]);
        free(node->val.block.children[i]);
        node->val.block.children[i] = NULL;
      }
      free(node->val.block.children);
      node->val.block.children = NULL;
      node->val.block.capacity = 0;
      node->val.block.count    = 0;
    }
    break;
  }
  case NODE_ASSIGNMENT:
  {
    if (node->val.assignment.left)
    {
      ast_free_node(node->val.assignment.left);
      free(node->val.assignment.left);
      node->val.assignment.left = NULL;
    }
    if (node->val.assignment.right)
    {
      ast_free_node(node->val.assignment.right);
      free(node->val.assignment.right);
      node->val.assignment.right = NULL;
    }
    break;
  }
  case NODE_FN_DECL:
  {
    if (node->val.fn_decl.name)
    {
      free(node->val.fn_decl.name);
      node->val.fn_decl.name = NULL;
    }
    if (node->val.fn_decl.params)
    {
      for (size_t i = 0; i < node->val.fn_decl.count; ++i)
      {
        free(node->val.fn_decl.params[i]);
        node->val.fn_decl.params[i] = NULL;
      }
      free(node->val.fn_decl.params);
      node->val.fn_decl.params = NULL;
    }
    break;
  }
  case NODE_FN_IMPL:
  {
    if (node->val.fn_impl.name)
    {
      free(node->val.fn_impl.name);
      node->val.fn_impl.name = NULL;
    }
    if (node->val.fn_impl.params)
    {
      for (size_t i = 0; i < node->val.fn_impl.count; ++i)
      {
        free(node->val.fn_impl.params[i]);
        node->val.fn_impl.params[i] = NULL;
      }
      free(node->val.fn_impl.params);
      node->val.fn_impl.params = NULL;
      node->val.fn_impl.capacity = 0;
      node->val.fn_impl.count = 0;
    }
    if (node->val.fn_impl.body)
    {
      ast_free_node(node->val.fn_impl.body);
      free(node->val.fn_impl.body);
      node->val.fn_impl.body = NULL;
    }
    break;
  }
  default:
    break;
  }
  
  return;
}

void ast_add_child_to_block(ast_node_t *block, ast_node_t *child)
{
  if (!block | !child) return;

  if (!block->val.block.children)
  {
    block->val.block.children  = malloc(sizeof(ast_node_t) * 8);
    block->val.block.capacity = 8;

    block->val.block.children[0] = child;
    block->val.block.count       = 1;
    return;
  }

  if (block->val.block.count >= block->val.block.capacity)
  {
    // Double the vector
    ast_node_t **new_vec = malloc(sizeof(ast_node_t*) * block->val.block.capacity * 2);
    memcpy(new_vec, block->val.block.children, block->val.block.count * sizeof(ast_node_t*));
    free(block->val.block.children);
    block->val.block.children = new_vec;
    block->val.block.capacity *= 2;
  }
  
  block->val.block.children[block->val.block.count] = child;
  block->val.block.count++;
  return;
}

void ast_add_arg_to_function(ast_node_t *fn_call, ast_node_t *arg)
{
  if (!fn_call | !arg) return;

  if (!fn_call->val.fn_call.args)
  {
    fn_call->val.fn_call.args  = malloc(sizeof(ast_node_t) * 8);
    fn_call->val.fn_call.capacity = 8;

    fn_call->val.fn_call.args[0] = arg;
    fn_call->val.fn_call.count       = 1;
    return;
  }

  if (fn_call->val.fn_call.count >= fn_call->val.fn_call.capacity)
  {
    // Double the vector
    ast_node_t **new_vec = malloc(sizeof(ast_node_t*) * fn_call->val.fn_call.capacity * 2);
    memcpy(new_vec, fn_call->val.fn_call.args, fn_call->val.fn_call.count * sizeof(ast_node_t*));
    free(fn_call->val.fn_call.args);
    fn_call->val.fn_call.args = new_vec;
    fn_call->val.fn_call.capacity *= 2;
  }
  
  fn_call->val.fn_call.args[fn_call->val.fn_call.count] = arg;
  fn_call->val.fn_call.count++;
  return;
}
