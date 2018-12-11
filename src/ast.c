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
    AST_NODE* node = malloc(sizeof(AST_NODE));
    node->type = type;
    node->value = NULL;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
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
    TOKEN *initial_token = get_next_token(ast_parser);
    if (initial_token != NULL) {
        ast_root = ast_parse_expression(ast_parser, initial_token);
    }
    return ast_root;
}

AST_NODE* ast_token_to_leaf_node(TOKEN *token)
{
    AST_NODE *node = NULL;

    if (token->type == TK_IDENTIFIER) {
        node = ast_allocate_node(AST_T_IDENTIFIER);
        node->value = ast_allocate_node_value_from_str((const char *) token->value);
    } else if (token->type == TK_NUMERIC) {
        node = ast_allocate_node(AST_T_NUMERIC);
        node->value = ast_allocate_node_value_from_str((const char *) token->value);
    }

    return node;
}

/**
 * Returns AST_T_INFIX_OPERATION or AST_T_NUMERIC.
 */
AST_NODE* ast_parse_numeric(AST_PARSER *parser, TOKEN *initial_token)
{
    AST_NODE *node_numeric = ast_token_to_leaf_node(initial_token);
    TOKEN *peak_token = peak_next_token(parser);
    if (peak_token != NULL && peak_token->type == TK_OPERATOR) {
        const char *operator = (const char *) get_next_token(parser)->value;
        AST_NODE *node_operation = ast_allocate_node(AST_T_INFIX_OPERATION);
        node_operation->value = ast_allocate_node_value_from_str(operator);
        node_operation->left = node_numeric;
        node_operation->right = ast_parse_expression(parser, get_next_token(parser));
        return node_operation;
    }
    return node_numeric;
}

/**
 * Returns AST_T_INFIX_OPERATION or AST_T_STRING.
 */
AST_NODE* ast_parse_string(AST_PARSER *parser, TOKEN *initial_token)
{
    AST_NODE *node_string = ast_token_to_leaf_node(initial_token);
    TOKEN *peak_token = peak_next_token(parser);
    if (peak_token != NULL && peak_token->type == TK_OPERATOR) {
        const char *operator = (const char *) get_next_token(parser)->value;
        AST_NODE *node_operation = ast_allocate_node(AST_T_INFIX_OPERATION);
        node_operation->value = ast_allocate_node_value_from_str(operator);
        node_operation->left = node_string;
        node_operation->right = ast_parse_expression(parser, get_next_token(parser));
        return node_operation;
    }
    return node_string;
}

/**
 * Returns AST_T_CALL or AST_T_IDENTIFIER
 */
AST_NODE* ast_parse_call(AST_PARSER *parser, TOKEN *initial_token)
{
    AST_NODE *node_identifier = ast_token_to_leaf_node(initial_token);
    TOKEN *peak_token = peak_next_token(parser);
    if (peak_token != NULL && AST_TOKEN_STARTS_EXPRESSION(peak_token)) {
        AST_NODE *node_call = ast_allocate_node(AST_T_CALL);
        node_call->left = node_identifier;
        node_call->right = ast_parse_expression(parser, get_next_token(parser));
        return node_call;
    }
    return node_identifier;
}

/*
 * Returns AST_NODE, the first AST_T_EXPRESSION child.
 */
AST_NODE* ast_parse_sub_expression(AST_PARSER *parser, TOKEN *initial_token)
{
    AST_NODE* node_expression = NULL;
    node_expression = ast_parse_expression(parser, get_next_token(parser)); // "<Expression>"
    TOKEN *peak_token = peak_next_token(parser);
    if (peak_token == NULL || peak_token->type != TK_PAREN_CLOSE) {
        exit_with_ast_parse_error("Missing closing ')'.");
    }
    get_next_token(parser); // ")"
    return node_expression;
}

int AST_TOKEN_STARTS_EXPRESSION(TOKEN *token) {
    return (
        token->type == TK_PAREN_OPEN
        || token->type == TK_IDENTIFIER
        || token->type == TK_NUMERIC
        || token->type == TK_STRING
    );
}
AST_NODE* ast_parse_expression(AST_PARSER *parser, TOKEN *initial_token)
{
    TOKEN_TYPE token_type = initial_token->type;
    if (!AST_TOKEN_STARTS_EXPRESSION(initial_token)) {
        return NULL;
    }

    AST_NODE *node_expression = ast_allocate_node(AST_T_EXPRESSION);   
    AST_NODE *child_expression = NULL;

    switch (token_type) {
        // <Numeric> [<Operator> <Expression>]
        case TK_NUMERIC:
            child_expression = ast_parse_numeric(parser, initial_token);
            break;
        
        // <String> [<Operator> <Expression>]
        case TK_STRING:
            child_expression = ast_parse_string(parser, initial_token);
            break;
        
        // "(" <Expression> ")"
        case TK_PAREN_OPEN:
            child_expression = ast_parse_sub_expression(parser, initial_token);
            break;
        
        // <Identifier> [<Expression>]
        case TK_IDENTIFIER:
            child_expression = ast_parse_call(parser, initial_token);
            break;
        
        default:
            fprintf(stderr, "Compiler Exception: AST_PARSE_EXPRESSION failed internally.\n");
    }

    TOKEN *peak_node = peak_next_token(parser);
    if (peak_node != NULL && peak_node->type == TK_OPERATOR) {
        // <child_expression> <Operator> <Expression>
        const char *operator = (const char *) get_next_token(parser)->value;
        AST_NODE *node_operation = ast_allocate_node(AST_T_INFIX_OPERATION);
        node_operation->value = ast_allocate_node_value_from_str(operator);
        node_operation->left = child_expression;
        node_operation->right = ast_parse_expression(parser, get_next_token(parser));
        node_expression->left = node_operation;
    } else if (peak_node != NULL && peak_node->type == TK_SEQUENCE) {
        // <child_expression> <Sequence> <Expression>
        get_next_token(parser);
        AST_NODE *node_sequence = ast_allocate_node(AST_T_SEQUENCE);
        node_sequence->left = child_expression;
        node_sequence->right = ast_parse_expression(parser, get_next_token(parser));
        node_expression->left = node_sequence;
    } else if (peak_node != NULL && AST_TOKEN_STARTS_EXPRESSION(peak_node)) {
        // <child_expression> <expression>
        AST_NODE *node_call = ast_allocate_node(AST_T_CALL);
        node_call->left = child_expression;
        node_call->right = ast_parse_expression(parser, get_next_token(parser));
        node_expression->left = node_call;
    } else {
        // Collapse expression nodes if possible.
        if (child_expression->type == AST_T_EXPRESSION) {
            free(node_expression);
            return child_expression;
        }

        node_expression->left = child_expression;
    }

    return node_expression;
}


char *ast_debug_type_to_name(AST_TYPE type)
{
    switch (type) {
        case AST_T_EXPRESSION:
            return strdup("EXPRESSION");
        case AST_T_INFIX_OPERATION:
            return strdup("INFIX_OPERATION");
        case AST_T_NUMERIC:
            return strdup("NUMERIC");
        case AST_T_CALL:
            return strdup("CALL");
        case AST_T_IDENTIFIER:
            return strdup("IDENTIFIER");
        case AST_T_SEQUENCE:
            return strdup("SEQUENCE");
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

    if (node->left != NULL) {
        ast_debug_node(node->left);
        printf(" \"%p\" -> \"%p\" [ taillabel=\"L\" ];\n", (void *) node, (void *) node->left);
    }

    if (node->right != NULL) {
        ast_debug_node(node->right);
        printf(" \"%p\" -> \"%p\" [ taillabel=\"R\" ];\n", (void *) node, (void *) node->right);
    }
}

void ast_debug_graph(AST_NODE *root)
{
    printf("digraph {\n");
    ast_debug_node(root);
    printf("}\n");
}
