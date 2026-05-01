// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#include "lang.h"
#include "codegen.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>

void cgcontext_init(cgcontext_t *cg, char *module_name)
{
  if (!cg) return;
  
  cg->mod     = LLVMModuleCreateWithName(module_name);
  cg->builder = LLVMCreateBuilder();
}

void cgcontext_free(cgcontext_t *cg)
{
  if (!cg) return;
  
  LLVMDisposeBuilder(cg->builder);
  LLVMDisposeModule(cg->mod);
}

//
// To generate LLVM IR, you need three main things:
//
// - Module: The container for your entire program (the file).
// - Builder: A "cursor" that knows where to insert instructions.
// - Symbol Table: A map that links your IDENT names to LLVM Value
//   pointers (memory locations).
//
// Each gen_X function generates code for a specific node in the AST,
// and returns a LLVMValueRef. This is the base class of all values
// computed by a program that may be used as operands to other values.
//
bool gen_program(ast_node_t *node, const char *outfile)
{
  cgcontext_t cg;
  cg.mod = LLVMModuleCreateWithName("povolang");

  //
  // Generate a simple function with LLVM, just for testing
  //
  // Add a function
  LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
  LLVMTypeRef ret_type =
    //               ret type         params       arity  param_count
    LLVMFunctionType(LLVMInt32Type(), param_types, 2,     0);
  LLVMValueRef sum = LLVMAddFunction(cg.mod, "sum", ret_type);
  // Entrypoint
  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);
  // Add instruction
  LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
  // Generate the return statement of the add
  LLVMBuildRet(builder, tmp);

  gen_node(&cg, node);

  char *error = NULL;
  LLVMVerifyModule(cg.mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);
  
  if (LLVMWriteBitcodeToFile(cg.mod, outfile) != 0)
  {
    fprintf(stderr, "error writing bitcode to file\n");
    return false;
  }
  
  return true;
}

LLVMValueRef gen_binary(cgcontext_t *cg, ast_node_t *node)
{
  assert(cg && node && node->type == NODE_BINARY_OP);
  
  LLVMValueRef left  = gen_node(cg, node->val.binary.left);
  LLVMValueRef right = gen_node(cg, node->val.binary.right);
  
  switch (node->val.binary.op)
  {
  case OP_ADD:
    return LLVMBuildAdd(cg->builder, left, right, "addtmp");
  case OP_SUB:
    return LLVMBuildSub(cg->builder, left, right, "subtmp");
  case OP_MUL:
    return LLVMBuildMul(cg->builder, left, right, "multmp");
  case OP_DIV:
    return LLVMBuildSDiv(cg->builder, left, right, "divtmp");
  case OP_EQ:
    return LLVMBuildICmp(cg->builder, LLVMIntEQ, left, right, "eqtmp");
  case OP_NEQ:
    return LLVMBuildICmp(cg->builder, LLVMIntNE, left, right, "neqtmp");
  case OP_GT:
    return LLVMBuildICmp(cg->builder, LLVMIntSGT, left, right, "gttmp");
  case OP_LT:
    return LLVMBuildICmp(cg->builder, LLVMIntSLT, left, right, "lttmp");
  default:
    fprintf(stderr, "Codegen error: unknown binary operator\n");
    return NULL;
  }
  
  return NULL;
}

LLVMValueRef gen_literal(cgcontext_t *cg, ast_node_t *node)
{
  assert(cg && node && node->type == NODE_LITERAL);

  switch (node->val.literal.type)
  {
  case LITERAL_VOID:
    break;
  case LITERAL_INT:
    return LLVMConstInt(LLVMInt32Type(), node->val.literal.val.integer, true);
  case LITERAL_FLOAT:
    return LLVMConstReal(LLVMDoubleType(), node->val.literal.val.floating);
  case LITERAL_BOOL:
    return LLVMConstInt(LLVMInt1Type(), node->val.literal.val.boolean, true);
  case LITERAL_STRING:
    return LLVMConstString(node->val.literal.val.string,
                           strlen(node->val.literal.val.string),
                           false);
  default:
    return NULL;
  }
  
  return NULL;
}

LLVMValueRef gen_variable(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_fn_call(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_if(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_for(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_while(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_assignment(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_block(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_fn_decl(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_fn_impl(cgcontext_t *cg, ast_node_t *node)
{
  (void) node;
  (void) cg;
  // TODO
  return NULL;
}

LLVMValueRef gen_node(cgcontext_t *cg, ast_node_t *node)
{
  if (!node) return NULL;

  switch (node->type)
  {
  case NODE_BINARY_OP:  return gen_binary(cg, node);
  case NODE_LITERAL:    return gen_literal(cg, node);
  case NODE_VARIABLE:   return gen_variable(cg, node);
  case NODE_FN_CALL:    return gen_fn_call(cg, node);
  case NODE_IF:         return gen_if(cg, node);
  case NODE_FOR:        return gen_for(cg, node);
  case NODE_WHILE:      return gen_while(cg, node);
  case NODE_ASSIGNMENT: return gen_assignment(cg, node);
  case NODE_BLOCK:      return gen_block(cg, node);
  case NODE_FN_DECL:    return gen_fn_decl(cg, node);
  case NODE_FN_IMPL:    return gen_fn_impl(cg, node);
  default:              return NULL;
  }
}
