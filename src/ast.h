
#include <stdint.h>
#include "./tokenizer.h"
#include "./data_structures.h"

#ifndef _H_AST_
#define _H_AST_


typedef enum {
    AST_T_EXPRESSION,
    AST_T_NUMERIC,
    AST_T_STRING,
    AST_T_CALL,
    AST_T_IDENTIFIER,
    AST_T_OPERATOR
} AST_TYPE;

typedef struct _ast_value {
    uint16_t    buffer_len; // Size of allocated buffer.
    uint16_t    data_len; // Number of bytes used to express value in buffer.
    uint8_t     *buffer;
} AST_VALUE;

typedef struct _ast_node {
    AST_TYPE            type;
    AST_VALUE           *value;
    struct _ast_node    *parent;
    struct _ast_node    *next;
    struct _ast_node    *prev;
    struct _ast_node    *children;
} AST_NODE;

typedef struct _ast_parser {
    TOKEN_PARSER    *token_parser;
} AST_PARSER;

TOKEN *peak_next_token(AST_PARSER *parser);
TOKEN* get_next_token(AST_PARSER *parser);

AST_NODE* ast_allocate_node(AST_TYPE type);
AST_VALUE* ast_allocate_node_value_from_str(const char *);
AST_NODE* ast_token_to_leaf_node(TOKEN *token);

void exit_with_ast_parse_error(const char *msg);

int AST_TOKEN_STARTS_EXPRESSION(TOKEN *token);

AST_NODE* ast_parse_call(AST_PARSER *parser);
AST_NODE* ast_parse_expression(AST_PARSER *parser);

AST_NODE* ast_parse_file(const char *src_path);

char *ast_debug_type_to_name(AST_TYPE type);
char *ast_debug_node_value_to_str(AST_TYPE type, AST_VALUE *value);

void ast_debug_node(AST_NODE *node);
void ast_debug_graph(AST_NODE *root);

#endif