#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#include "./ast.h"
#include "./code_generator.h"
#include "code_includes/fl_directives.h"

void cg_append_line(CG_SOURCE_CODE *code, const char *line, ...) {
    va_list args;
    va_start(args, line);
    vfprintf(code->output_file, line, args);
    fprint(code->output_file, "\n");
    va_end(args);
}

void cg_visit_closure(CG_SOURCE_CODE *code, AST_NODE *node) {
    /*

    _FL_SYMBOL_TABLE* ${_FL_S_CLOSURE_PREFIX}_${node::int} (_FL_OBJECT_STRUCT object) {
        _FL_SYMBOL_TABLE symbols = _FL_S_INIT_SYMBOL_TABLE();

        return _FL_SYMBOL_TABLE* or NULL;
    }

    */

    cg_append_line(code, "void ##_FL_CLOSURE_PREFIX##_%d (##_FL_OBJECT_STRUCT## *object) {", node);
    cg_append_line(code, "\t" "_FL_SYMBOL_TABLE symbols = _FL_S_INIT_SYMBOL_TABLE();");

    // AST_NODE *child = NULL;
    // while ((child = ) != NULL) {

    // }

    // for (children_closures in node) {
    //     cg_append_line(code, "\t" "insert_symbol(&symbols, \"%s\", %d);", );
    // }

    // potential calls here...

    cg_append_line(code, "}");
}

void cg_visit_call(CG_SOURCE_CODE *code, AST_NODE *node) {
    /*

    _FL_OBJECT_STRUCT object = _FL_S_INIT_OBJECT_STRUCT();
    // populate object from args, potentially from return value of call

    ${_FL_S_CLOSURE_PREFIX}_${node_closure::int}(object);

    */
}

void ast_node_to_source_code(CG_SOURCE_CODE *code, AST_NODE *node) {
    void (*cg_node_visitor)(CG_SOURCE_CODE*,AST_NODE*) = NULL;

    switch (node->type) {
        case AST_T_CALL:
            cg_node_visitor = cg_visit_call;
            break;
        case AST_T_CLOSURE:
            cg_node_visitor = cg_visit_closure;
            break;
        default:
            cg_node_visitor = NULL;
    }

    if (cg_node_visitor == NULL) {
        fprintf(stderr, "Missing visitor function.\n");
        exit(EXIT_FAILURE);
    }

    cg_node_visitor(code, node);
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
