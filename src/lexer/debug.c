#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "./token_types.h"
#include "./debug.h"

char *lexer_token_type_to_name(TOKEN_TYPE type)
{
    switch (type) {
        case TK_LINE_COMMENT:
            return strdup("LINE_COMMENT");
        case TK_NEWLINE:
            return strdup("NEWLINE");
        case TK_STRING:
            return strdup("STRING");
        case TK_IDENTIFIER:
            return strdup("IDENTIFIER");
        case TK_PAREN_OPEN:
            return strdup("PAREN_OPEN");
        case TK_PAREN_CLOSE:
            return strdup("PAREN_CLOSE");
        case TK_OBJECT_OPEN:
            return strdup("PARAM_OPEN");
        case TK_OBJECT_CLOSE:
            return strdup("PARAM_CLOSE");
        case TK_CLOSURE:
            return strdup("CLOSURE");
        case TK_NUMERIC:
            return strdup("NUMERIC");
        default:
            return strdup("UNKNOWN TYPE");
    }
}

char *lexer_token_value_to_str(TOKEN_TYPE type, uint8_t *value)
{
    if (value == NULL) return strdup("<NO VALUE>");
    switch (type) {
        case TK_NEWLINE:
            return strdup("<NEWLINE>");
        case TK_STRING:
        case TK_IDENTIFIER:
        case TK_NUMERIC:
        case TK_LINE_COMMENT:
            return strdup((const char *) value);
        default:
            return strdup("<UNKNOWN TYPE>");
    }
}

void lexer_debug_token(TOKEN *token)
{
    char *token_name = lexer_token_type_to_name(token->type);
    char *token_value = lexer_token_value_to_str(token->type, token->buffer);
    printf("(TOKEN) %s: %s\n", token_name, token_value);
    free(token_name);
    free(token_value);
}