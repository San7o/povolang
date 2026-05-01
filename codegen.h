// SPDX-License-Identifier: MIT
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// Github:  @San7o

#ifndef LANG_CODEGEN_H
#define LANG_CODEGEN_H

#include <llvm-c/Core.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Analysis.h>

// Used internally to save LLVM state
typedef struct codegen {
  LLVMModuleRef     mod;
  LLVMBuilderRef    builder;
} cgcontext_t;

void cgcontext_init(cgcontext_t *cg, char *module_name);
void cgcontext_free(cgcontext_t *cg);

LLVMValueRef gen_node(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_fn_impl(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_fn_decl(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_block(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_assignment(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_while(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_for(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_if(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_fn_call(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_variable(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_literal(cgcontext_t *cg, ast_node_t *node);
LLVMValueRef gen_binary(cgcontext_t *cg, ast_node_t *node);

#endif // LANG_CODEGEN_H
