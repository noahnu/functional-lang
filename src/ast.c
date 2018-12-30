#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./ast.h"
#include "./lexer/token_types.h"
#include "./lexer/lexer.h"

TOKEN* peak_next_token(AST_PARSER *parser)
{
    return parser->token_parser->token_cur;
}

TOKEN* get_next_token(AST_PARSER *parser)
{
    TOKEN *token = NULL;
    if (parser->token_parser->token_cur != NULL) {
        token = parser->token_parser->token_cur;
        parser->token_parser->token_cur = parser->token_parser->token_cur->next;
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
    AST_NODE *node = malloc(sizeof(AST_NODE));
    node->type = type;
    node->value = NULL;
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->children = NULL;
    node->rel_token = NULL;
    node->closure = NULL;
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
    ast_parser->token_parser = lexer_init_parser(src_path);

    AST_NODE *ast_root = NULL;
    TOKEN *initial_token = peak_next_token(ast_parser);
    if (initial_token != NULL) {
        ast_root = ast_parse_expression(ast_parser, NULL);
    }
    return ast_root;
}

AST_NODE* ast_parse_subexpression(AST_PARSER *parser, AST_NODE *closure)
{
    AST_NODE *node = NULL;

    // <Callee> | <Identifier>
    if (peak_next_token(parser) != NULL) {
        node = ast_allocate_node(AST_T_CALL);
        node->children = ast_parse_expression(parser, closure);
    } else {
        return NULL;
    }

    TOKEN *peak_token = peak_next_token(parser);
    if (peak_token != NULL) {
        // <Right_Expression_1> <_2> <_3> ... <_n>
        AST_NODE *rightmost_child = node->children;
        while (peak_next_token(parser) != NULL) {
            AST_NODE *node_arg = ast_parse_expression(parser, closure);
            if (node_arg == NULL) break;

            node_arg->prev = rightmost_child;
            rightmost_child->next = node_arg;
            rightmost_child = node_arg;
        }
    }

    // No arguments.
    if (node->children->next == NULL) {
        AST_NODE *node_function = node->children;
        free(node);
        return node_function;
    }

    return node;
}

AST_NODE *ast_transform_subexpression(AST_PARSER *parser, AST_NODE *subexpression) {
    if (subexpression == NULL || subexpression->type != AST_T_IDENTIFIER) {
        return subexpression;
    }

    TOKEN *peak_token = peak_next_token(parser);
    if (peak_token->type != TK_OBJECT_OPEN && peak_token->type != TK_CLOSURE) {
        return subexpression;
    }

    // Children: [<Identifier>, <Param List>, <Body Expression>]
    AST_NODE *closure = ast_allocate_node(AST_T_CLOSURE);
    closure->closure = subexpression->closure; // parent closure from original subexpression
    subexpression->closure = NULL; // no need to keep link since this is a function def.
    closure->children = subexpression; // <Identifier>

    AST_NODE *param_list = ast_allocate_node(AST_T_OBJECT);
    subexpression->next = param_list;
    if (peak_token->type == TK_OBJECT_OPEN) {
        param_list->rel_token = get_next_token(parser); // consume "{"
        AST_NODE *rightmost_child = param_list->children;
        while (peak_next_token(parser) != NULL) {
            AST_NODE *node_arg = ast_parse_expression(parser, NULL);
            if (node_arg == NULL) break;
            if (node_arg->type != AST_T_IDENTIFIER) {
                exit_with_ast_parse_error("Only identifiers allowed in closure argument list.");
            }

            node_arg->prev = rightmost_child;
            if (rightmost_child != NULL) {
                rightmost_child->next = node_arg;
            } else {
                param_list->children = node_arg;
            }
            rightmost_child = node_arg;
        }
        get_next_token(parser); // consume "}"
    }

    peak_token = peak_next_token(parser);
    if (peak_token->type != TK_CLOSURE) {
        exit_with_ast_parse_error("Expected '->' in closure definition.");
    }

    get_next_token(parser); // consume '->'
    param_list->next = ast_parse_expression(parser, closure); // <Body Expr>

    return closure;
}

AST_NODE* ast_parse_expression(AST_PARSER *parser, AST_NODE *closure)
{
    TOKEN *peak_token = peak_next_token(parser);

    // Base cases: Leaf nodes of AST
    if (
        peak_token->type == TK_IDENTIFIER ||
        peak_token->type == TK_STRING ||
        peak_token->type == TK_NUMERIC
    ) {
        AST_NODE *node = ast_token_to_leaf_node(get_next_token(parser));
        node->closure = closure;
        return node;
    }

    // "(" <Expression> ")"
    if (peak_token->type == TK_PAREN_OPEN) {
        get_next_token(parser); // consume "("
        AST_NODE *sub_expression = ast_parse_subexpression(parser, closure);
        sub_expression = ast_transform_subexpression(parser, sub_expression);
        peak_token = peak_next_token(parser);
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

    node->rel_token = token;
    node->value = ast_allocate_node_value_from_str((const char *) token->buffer);
    return node;
}
