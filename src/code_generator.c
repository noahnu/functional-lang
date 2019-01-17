#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./ast.h"
#include "./code_generator.h"

// TODO:
// - Create a "closure" (gets a unique name connected to identifier and lexical scope)
// - Append instructions to the closure.
// TODO: must be some way to translate the closure to actual source code or assembly.
// essentially below function will flatten the AST

CG_CLOSURE *cg_init_closure(AST_NODE *node_closure) {
    CG_CLOSURE *cg_closure = malloc(sizeof(CG_CLOSURE));
    cg_closure->parent = NULL;
    cg_closure->variables = NULL;
    cg_closure->body = NULL;
    return cg_closure;
}

void ast_node_to_source_code(CG_SOURCE_CODE *code, AST_NODE *node) {
    if (node->type == AST_T_CLOSURE) {
        CG_CLOSURE *closure = cg_init_closure(node);
        cg_code_add_closure(code, closure);
    }
}

void cg_code_add_closure(CG_SOURCE_CODE *code, CG_CLOSURE *closure) {
    // TODO: add closure to linked list
}

CG_SOURCE_CODE *cg_init_source_code() {
    CG_SOURCE_CODE *code = malloc(sizeof(CG_SOURCE_CODE));
    code->output_file = NULL;
    return code;
}

void generate_source_code(const char *output_path, AST_NODE *ast_root, SOURCE_CODE_OPTS *opts) {
    // Post-order traversal of AST.

    CG_SOURCE_CODE *code = cg_init_source_code();
    code->output_file = fopen(output_path, 'w');

    ast_node_to_source_code(code, ast_root);

    fclose(code->output_file);
}
