#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>

#include "./ast.h"
#include "./ast_debug.h"
#include "./code_generator.h"
#include "./linked_list.h"
#include "./logging.h"

#define CG_INDENT "  "

void visit_node(
    CG_SOURCE_CODE *code,
    AST_NODE *root,
    int (*predicate)(AST_NODE*),
    void (*node_visitor)(CG_SOURCE_CODE*, AST_NODE*)
) {
    // TODO: change to iterative approach; topological order
    if (root->children) {
        AST_NODE *child = root->children;
        while (child != NULL) {
            visit_node(code, child, predicate, node_visitor);
            child = child->next;
        }
    }
    if (predicate(root)) {
        node_visitor(code, root);
    }
}

void cg_append_line(CG_SOURCE_CODE *code, const char *line, ...) {
    va_list args;
    va_start(args, line);
    vfprintf(code->output_file, line, args);
    fprintf(code->output_file, "\n");
    va_end(args);
}

void cg_visit_call(CG_SOURCE_CODE *code, AST_NODE *node) {
    AST_NODE *identifier = node->children;
    AST_NODE *arg_list = identifier->next;
    int argc = ast_children_len(node) - 1;
    cg_append_line(code, "// CALL");
    cg_append_line(code, "_FL_RESULT_STRUCT* call_%p() {", node);

    cg_append_line(
        code,
        CG_INDENT "_FL_DATUM *data = malloc(sizeof(_FL_DATUM) * %d);",
        argc + 1
    );
    AST_NODE *arg = arg_list;
    int i = 0;
    while (arg != NULL) {
        cg_append_line(
            code,
            CG_INDENT "data[%d] = NULL;", // how to copy value?
            i
        );
        i++;
        arg = arg->next;
    }
    cg_append_line(code, CG_INDENT "data[%d] = NULL;", argc);

    cg_append_line(code, "}" "\n", node);
}

void cg_visit_closure(CG_SOURCE_CODE *code, AST_NODE *node) {
    AST_NODE *arg_list = node->children->next;
    int argc = ast_children_len(arg_list);

    char *closure_name = ast_debug_node_value_to_str(
        AST_T_CLOSURE, node->children->value);
    
    cg_append_line(code, "// CLOSURE: %s", closure_name);
    cg_append_line(code, "_FL_RESULT_STRUCT* closure_%p() {", node);

    cg_append_line(
        code,
        CG_INDENT "_FL_CLOSURE_STRUCT *closure = _FL_INIT_CLOSURE();" "\n"
        CG_INDENT "closure->symbols = malloc(sizeof(_FL_SYMBOL_TABLE) * %d);",
        argc + 1
    );

    AST_NODE *arg = arg_list->children;
    int i = 0;
    while (arg != NULL) {
        cg_append_line(
            code,
            CG_INDENT "closure->symbols[%d] = _FL_INIT_SYMBOL("
            "\"%s\"" ", " "DT_UNKNOWN" ", " "%d" ");",
            i,
            arg->value->buffer,
            0
        );
        i++;
        arg = arg->next;
    }
    cg_append_line(code, CG_INDENT "closure->symbols[%d] = NULL;", argc);



    cg_append_line(code, "}" "\n", node);

    free(closure_name);
}

void ast_node_to_source_code(CG_SOURCE_CODE *code, AST_NODE *node) {
    visit_node(code, node, IS_AST_T_CLOSURE, cg_visit_closure);
    visit_node(code, node, IS_AST_T_CALL, cg_visit_call);
}


CG_SOURCE_CODE *cg_init_source_code() {
    CG_SOURCE_CODE *code = malloc(sizeof(CG_SOURCE_CODE));
    code->output_file = NULL;
    return code;
}

void generate_source_code(const char *output_path, AST_NODE *ast_root, SOURCE_CODE_OPTS *opts) {
    // Post-order traversal of AST.

    DEBUG_PRINT("Generating...");

    CG_SOURCE_CODE *code = cg_init_source_code();
    code->output_file = fopen(output_path, "w");

    DEBUG_PRINT("Writing to %s", output_path);

    ast_node_to_source_code(code, ast_root);

    fclose(code->output_file);

    DEBUG_PRINT("Finished source code generation.");
}
