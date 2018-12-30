#include <stdint.h>
#include <stdbool.h>
#include "./lexer/lexer.h"
#include "./ast.h"

#ifndef _H_AST_DEBUG_
#define _H_AST_DEBUG_

typedef struct _ast_debug_opts {
    bool    include_closure_ptrs;
    bool    include_token_location;
    bool    include_node_value;
} AST_DEBUG_OPTS;

char *ast_debug_type_to_name(AST_TYPE type);
char *ast_debug_node_value_to_str(AST_TYPE type, AST_VALUE *value);

void ast_debug_node(AST_NODE *node, AST_DEBUG_OPTS *opts);
void ast_debug_graph(AST_NODE *root, AST_DEBUG_OPTS *opts);

#endif