
#include <stdint.h>
#include "./lexer/lexer.h"

#ifndef _H_AST_
#define _H_AST_

typedef enum {
    AST_T_EXPRESSION,
    AST_T_NUMERIC,
    AST_T_STRING,
    AST_T_CALL,
    AST_T_IDENTIFIER,
    AST_T_OBJECT,
    AST_T_CLOSURE
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
    struct _ast_node    *closure;
    TOKEN               *rel_token;
} AST_NODE;

typedef struct _ast_parser {
    LEXER    *token_parser;
} AST_PARSER;

TOKEN *peak_next_token(AST_PARSER *parser);
TOKEN* get_next_token(AST_PARSER *parser);

AST_NODE* ast_allocate_node(AST_TYPE type);
AST_VALUE* ast_allocate_node_value_from_str(const char *);
AST_NODE* ast_token_to_leaf_node(TOKEN *token);

void exit_with_ast_parse_error(const char *msg);

AST_NODE *ast_transform_subexpression(AST_PARSER *parser, AST_NODE *subexpression);
AST_NODE* ast_parse_subexpression(AST_PARSER *parser, AST_NODE *closure);
AST_NODE* ast_parse_expression(AST_PARSER *parser, AST_NODE *closure);

AST_NODE* ast_parse_file(const char *src_path);

int IS_AST_T_EXPRESSION(AST_NODE *node);
int IS_AST_T_CALL(AST_NODE *node);
int IS_AST_T_OBJECT(AST_NODE *node);
int IS_AST_T_CLOSURE(AST_NODE *node);

int ast_children_len(AST_NODE *node);

#endif