// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "../lang.h"
#include "../codegen.h"
#include "micro-tests.h"

#include <string.h>

TEST(codegen_tests, add)
{
  parser_t p;
  cgcontext_t cg;
  cgcontext_init(&cg, "test");
  
  parser_init_from_input(&p, "5 + 6;");

  ast_node_t *node = parse_arith_expr(&p);
  ASSERT(node != NULL);
  ASSERT(node->type == NODE_BINARY_OP);
  ASSERT(node->val.binary.op == OP_ADD);
  ASSERT(node->val.binary.left != NULL);
  ASSERT(node->val.binary.left->type == NODE_LITERAL);
  ASSERT(node->val.binary.left->val.literal.type == LITERAL_INT);
  ASSERT(node->val.binary.left->val.literal.val.integer == 5);
  ASSERT(node->val.binary.right != NULL);
  ASSERT(node->val.binary.right->type == NODE_LITERAL);
  ASSERT(node->val.binary.right->val.literal.type == LITERAL_INT);
  ASSERT(node->val.binary.right->val.literal.val.integer == 6);
  
  LLVMValueRef result = gen_node(&cg, node);
  ASSERT(result != NULL);

  if (LLVMIsAConstantInt(result))
  {
    ASSERT(LLVMConstIntGetSExtValue(result) == 11);
  }
  else if (LLVMIsAInstruction(result))
  {
    ASSERT(LLVMGetInstructionOpcode(result) == LLVMAdd);
    
    LLVMValueRef op0 = LLVMGetOperand(result, 0);
    LLVMValueRef op1 = LLVMGetOperand(result, 1);
    
    ASSERT(LLVMIsAConstantInt(op0));
    ASSERT(LLVMConstIntGetSExtValue(op0) == 5);
    ASSERT(LLVMConstIntGetSExtValue(op1) == 6);
  }
  else
  {
    ASSERT(false && "Result was neither a constant nor an instruction");
  }

  cgcontext_free(&cg);
  TEST_SUCCESS;
}
