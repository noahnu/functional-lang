#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./ast.h"
#include "./tokenizer.h"
#include "./data_structures.h"


TOKEN* peak_next_token(AST_PARSER *parser)
{
    LINKED_LIST_ITEM *token_cur = parser->token_parser->token_cur;
    if (token_cur != NULL) {
        return (TOKEN *) token_cur->value;
    }
    return NULL;
}

TOKEN* get_next_token(AST_PARSER *parser)
{
    LINKED_LIST_ITEM *token_item = NULL;
    TOKEN *token = NULL;
    if (parser->token_parser->token_cur != NULL) {
        token_item = parser->token_parser->token_cur;
        parser->token_parser->token_cur = parser->token_parser->token_cur->next;
        token = (TOKEN *) token_item->value;
        return token;
    }
    return NULL;
}

void exit_with_ast_parse_error(const char *msg)
{
    fprintf(stderr, "AST Parse Error: ");
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

AST_NODE* ast_allocate_node(AST_TYPE type)
{
    void *node = NULL;
    if (type == AST_T_CALL) {
        node = malloc(sizeof(AST_NARY_NODE));
        LINKED_LIST_ITEM *children = malloc(sizeof(LINKED_LIST_ITEM));
        children->prev = NULL;
        children->next = NULL;
        children->value = NULL;
        ((AST_NARY_NODE *) node)->children = children;
    } else {
        node = malloc(sizeof(AST_BINARY_NODE));
        ((AST_BINARY_NODE *) node)->left = NULL;
        ((AST_BINARY_NODE *) node)->right = NULL;
    }

    ((AST_NODE *) node)->type = type;
    ((AST_NODE *) node)->value = NULL;
    ((AST_NODE *) node)->parent = NULL;

    return node;
}

AST_VALUE* ast_allocate_node_value_from_str(const char *str)
{
    AST_VALUE *value = malloc(sizeof(AST_VALUE));
    value->data_len = strlen(str) + 1; // +1 for \0
    value->buffer_len = value->data_len;
    value->buffer = malloc(value->buffer_len);
    memcpy(value->buffer, str, value->data_len - 1);
    value->buffer[value->data_len - 1] = 0;
    return value;
}

AST_PARSER* ast_init_parser()
{
    AST_PARSER *parser = malloc(sizeof(AST_PARSER));
    parser->token_parser = NULL;
    return parser;
}

AST_NODE* ast_parse_file(const char *src_path)
{
    AST_PARSER *ast_parser = ast_init_parser();
    ast_parser->token_parser = tokenizer_init_parser(src_path);

    AST_NODE *ast_root = NULL;
    TOKEN *initial_token = peak_next_token(ast_parser);
    if (initial_token != NULL) {
        ast_root = ast_parse_expression(ast_parser);
    }
    return ast_root;
}

AST_NODE* ast_parse_call(AST_PARSER *parser)
{
    AST_NARY_NODE *node = NULL;

    // <Callee>
    if (peak_next_token(parser) != NULL) {
        node = (AST_NARY_NODE *) ast_allocate_node(AST_T_CALL);
        node->children->value = (void *) ast_parse_expression(parser);
    } else {
        return NULL;
    }

    // <Right_Expression_1> <_2> <_3> ... <_n>
    LINKED_LIST_ITEM *rightmost_child = node->children;
    while (peak_next_token(parser) != NULL) {
        AST_NODE *node_arg = ast_parse_expression(parser);
        if (node_arg == NULL) break;

        LINKED_LIST_ITEM *arg_item = malloc(sizeof(LINKED_LIST_ITEM));
        arg_item->value = (void *) node_arg;
        arg_item->prev = rightmost_child;
        arg_item->next = NULL;
        rightmost_child->next = arg_item;
        rightmost_child = arg_item;
    }

    // No arguments.
    if (node->children->next == NULL) {
        AST_NODE *node_function = (AST_NODE *) node->children->value;
        free(node);
        return node_function;
    }

    return (AST_NODE *) node;
}

AST_NODE* ast_parse_expression(AST_PARSER *parser)
{
    TOKEN *peaked_token = peak_next_token(parser);
    TOKEN_TYPE token_type = peaked_token->type;

    // Base cases: Leaf nodes of AST
    if (
        token_type == TK_IDENTIFIER ||
        token_type == TK_STRING ||
        token_type == TK_NUMERIC
    ) {
        return ast_token_to_leaf_node(get_next_token(parser));
    }

    // "(" <Expression> ")"
    if (token_type == TK_PAREN_OPEN) {
        get_next_token(parser); // consume "("
        AST_NODE *sub_expression = ast_parse_call(parser);
        TOKEN *peak_token = peak_next_token(parser);
        if (peak_token == NULL || peak_token->type != TK_PAREN_CLOSE) {
            exit_with_ast_parse_error("Expected closing ')'.");
        }
        get_next_token(parser); // consume ")"
        return sub_expression;
    }
    
    return NULL;
}

AST_NODE* ast_token_to_leaf_node(TOKEN *token)
{
    AST_NODE *node = NULL;

    switch (token->type) {
        case TK_IDENTIFIER:
            node = ast_allocate_node(AST_T_IDENTIFIER);
            break;
        case TK_NUMERIC:
            node = ast_allocate_node(AST_T_NUMERIC);
            break;
        case TK_STRING:
            node = ast_allocate_node(AST_T_STRING);
            break;
        default:
            return NULL;
    }

    node->value = ast_allocate_node_value_from_str((const char *) token->value);
    return node;
}

// DEBUG functions

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

void ast_debug_node(AST_NODE *node)
{
    char *node_type = ast_debug_type_to_name(node->type);
    char *node_value = ast_debug_node_value_to_str(node->type, node->value);
    printf(" \"%p\" [ label=\"%s", (void *) node, node_type);
    if (node_value != NULL) {
        printf(": %s", node_value);
    }
    printf("\"];\n");
    if (node_type != NULL) free(node_type);
    if (node_value != NULL) free(node_value);

    if (node->type != AST_T_CALL) {
        AST_BINARY_NODE *binary_node = ((AST_BINARY_NODE *) node);
        if (binary_node->left != NULL) {
            ast_debug_node(binary_node->left);
            printf(" \"%p\" -> \"%p\" [ taillabel=\"L\" ];\n", (void *) node, (void *) binary_node->left);
        }

        if (binary_node->right != NULL) {
            ast_debug_node(binary_node->right);
            printf(" \"%p\" -> \"%p\" [ taillabel=\"R\" ];\n", (void *) node, (void *) binary_node->right);
        }
    } else {
        AST_NARY_NODE *nary_node = (AST_NARY_NODE *) node;
        LINKED_LIST_ITEM *nary_item = nary_node->children;
        while (nary_item != NULL && nary_item->value != NULL) {
            AST_NODE *child = (AST_NODE *) nary_item->value;
            ast_debug_node(child);
            printf(" \"%p\" -> \"%p\" ;\n", (void *) node, (void *) child);
            nary_item = nary_item->next;
        }
    }
}

void ast_debug_graph(AST_NODE *root)
{
    printf("digraph {\n");
    ast_debug_node(root);
    printf("}\n");
}
