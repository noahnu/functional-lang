#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./ast.h"
#include "./ast_debug.h"
#include "./lexer/token_types.h"
#include "./lexer/lexer.h"

char *ast_debug_type_to_name(AST_TYPE type)
{
    switch (type) {
        case AST_T_EXPRESSION:
            return strdup("EXPRESSION");
        case AST_T_NUMERIC:
            return strdup("NUMERIC");
        case AST_T_CALL:
            return strdup("CALL");
        case AST_T_IDENTIFIER:
            return strdup("IDENTIFIER");
        case AST_T_OBJECT:
            return strdup("OBJECT");
        case AST_T_CLOSURE:
            return strdup("CLOSURE");
        case AST_T_STRING:
            return strdup("STRING");
        default:
            return strdup("UNKNOWN_TYPE");
    }
}

char *ast_debug_node_value_to_str(AST_TYPE type, AST_VALUE *value)
{
    if (value == NULL || value->buffer == NULL) {
        return NULL;
    }
    return strdup((const char *) value->buffer);
}

void ast_debug_node(AST_NODE *node, AST_DEBUG_OPTS *opts)
{
    char *node_type = ast_debug_type_to_name(node->type);
    char *node_value = ast_debug_node_value_to_str(node->type, node->value);
    printf(" \"%p\" [ label=\"", (void *) node);
    if (opts->include_token_location && node->rel_token != NULL) {
        printf(
            "%s#%d:%d\n\n",
            node->rel_token->file_path,
            node->rel_token->lineno,
            node->rel_token->colno
        );
    }
    printf("%s", node_type);
    if (opts->include_node_value && node_value != NULL) {
        printf("\nvalue: '%s'", node_value);
    }
    printf("\"];\n");
    if (node_type != NULL) free(node_type);
    if (node_value != NULL) free(node_value);

    int pos = 0;
    AST_NODE *child = node->children;
    while (child != NULL) {
        ast_debug_node(child, opts);
        printf(" \"%p\" -> \"%p\" [ taillabel=\"%d\" ];\n", (void *) node, (void *) child, pos);
        child = child->next;
        pos += 1;
    }

    if (opts->include_closure_ptrs && node->closure != NULL) {
        printf(" \"%p\" -> \"%p\" [ style=dashed, color=grey ];\n", (void *) node, (void *) node->closure);
    }
}

void ast_debug_graph(AST_NODE *root, AST_DEBUG_OPTS *opts)
{
    printf("digraph {\n");
    ast_debug_node(root, opts);
    printf("}\n");
}