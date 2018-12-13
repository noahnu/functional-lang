
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./tokenizer.h"
#include "./data_structures.h"

int CHAR_IS_LETTER(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int CHAR_IS_DIGIT(char c) {
    return c >= '0' && c <= '9';
}

int CHAR_IS_NUMERIC_SEPARATOR(char c) {
    return c == '.';
}

int CHAR_IS_VALID_IDENTIFIER_SYMBOL(char c) {
    return c == '_' || c == '$';
}

int CHAR_IS_VALID_IDENTIFIER_ENTITY(char c) {
    return CHAR_IS_VALID_IDENTIFIER_SYMBOL(c)
        || CHAR_IS_LETTER(c) || CHAR_IS_DIGIT(c);
}

int CHAR_IS_NEWLINE(char c) {
    return c == '\n' || c == '\r';
}

int CHAR_IS_WHITESPACE(char c) {
    return c == ' ' || c == '\t';
}

int CHAR_IS_STRING_DELIMITER(char c) {
    return c == '`';
}

int CHAR_IS_PAREN_OPEN(char c) {
    return c == '(';
}

int CHAR_IS_PAREN_CLOSE(char c) {
    return c == ')';
}

int CHAR_IS_OPERATOR(char c) {
    return c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '>' || c == '<';
}

int CHAR_IS_COMMENT_START(char c) {
    return c == '#';
}

int INCLUDE_TOKEN_BOUNDARY_IN_VALUE(TOKEN_TYPE type) {
    return type != TK_STRING && type != TK_LINE_COMMENT;
}

int TOKEN_TYPE_SUPPORTS_VALUE(TOKEN_TYPE type) {
    return type != TK_PAREN_OPEN && type != TK_PAREN_CLOSE;
}

int TOKEN_LACKS_SEMANTIC_MEANING(TOKEN_TYPE token_type)
{
    switch (token_type)
    {
        case TK_NEWLINE:
        case TK_BLOCK_COMMENT:
        case TK_LINE_COMMENT:
            return 1;
        default:
            return 0;
    }
}

TOKEN_TYPE tokenizer_identify_by_leading_char(uint8_t c)
{
    if (CHAR_IS_COMMENT_START(c)) return TK_LINE_COMMENT;
    if (CHAR_IS_NEWLINE(c)) return TK_NEWLINE;
    if (CHAR_IS_PAREN_OPEN(c)) return TK_PAREN_OPEN;
    if (CHAR_IS_PAREN_CLOSE(c)) return TK_PAREN_CLOSE;
    if (CHAR_IS_STRING_DELIMITER(c)) return TK_STRING;
    if (CHAR_IS_OPERATOR(c)) return TK_OPERATOR;
    if (CHAR_IS_DIGIT(c)) return TK_NUMERIC;
    if (CHAR_IS_VALID_IDENTIFIER_SYMBOL(c) || CHAR_IS_LETTER(c)) {
        return TK_IDENTIFIER;
    }
    return TK_UNSET;
}

int tokenizer_char_member_of(TOKEN_TYPE token_type, uint8_t c)
{
    switch (token_type) {
        case TK_LINE_COMMENT:
            return !CHAR_IS_NEWLINE(c);
        case TK_NEWLINE:
            return CHAR_IS_NEWLINE(c);
        case TK_STRING:
            return !CHAR_IS_STRING_DELIMITER(c);
        case TK_OPERATOR:
            return CHAR_IS_OPERATOR(c);
        case TK_IDENTIFIER:
            return CHAR_IS_VALID_IDENTIFIER_ENTITY(c);
        case TK_NUMERIC:
            return CHAR_IS_DIGIT(c) || CHAR_IS_NUMERIC_SEPARATOR(c);
        default:
            return 0;
    }
}

int tokenizer_char_is_terminal(TOKEN_TYPE token_type, uint8_t c)
{
    switch (token_type) {
        case TK_STRING:
            return CHAR_IS_STRING_DELIMITER(c);
        case TK_LINE_COMMENT:
            return CHAR_IS_NEWLINE(c);
        default:
            return 0;
    }
}

void tokenizer_free_token(TOKEN *token)
{
    if (token->value != NULL) free(token->value);
    free(token);
}

char *tokenizer_token_type_to_name(TOKEN_TYPE type)
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
        case TK_OPERATOR:
            return strdup("OPERATOR");
        case TK_NUMERIC:
            return strdup("NUMERIC");
        default:
            return strdup("UNKNOWN TYPE");
    }
}

char *tokenizer_token_value_to_str(TOKEN_TYPE type, uint8_t *value)
{
    if (value == NULL) return strdup("<NO VALUE>");
    switch (type) {
        case TK_NEWLINE:
            return strdup("<NEWLINE>");
        case TK_STRING:
        case TK_IDENTIFIER:
        case TK_OPERATOR:
        case TK_NUMERIC:
        case TK_LINE_COMMENT:
            return strdup((const char *) value);
        default:
            return strdup("<UNKNOWN TYPE>");
    }
}

void tokenizer_debug_token(TOKEN *token)
{
    char *token_name = tokenizer_token_type_to_name(token->type);
    char *token_value = tokenizer_token_value_to_str(token->type, token->value);
    printf("(TOKEN) %s: %s\n", token_name, token_value);
    free(token_name);
    free(token_value);
}

TOKEN* tokenizer_read_token(FILE *src)
{
    uint16_t buffer_len = 32;
    uint8_t *token_value = (uint8_t *) malloc(sizeof(uint8_t) * buffer_len);
    uint8_t token_value_len = 0;
    token_value[0] = 0;

    TOKEN_TYPE token_type = TK_UNSET;
    uint8_t c;
    int i_c;
    while ( (i_c = fgetc(src)) != EOF ) {
        c = (uint8_t) i_c;
        if (token_type == TK_UNSET) {
            if (CHAR_IS_WHITESPACE(c)) continue;

            token_type = tokenizer_identify_by_leading_char(c);
            if (token_type == TK_UNSET) {
                fprintf(stderr, "Error tokenizing source code. Invalid character encountered.\n");
                return NULL;
            }

            if (!TOKEN_TYPE_SUPPORTS_VALUE(token_type)) {
                break;
            }

            if (!INCLUDE_TOKEN_BOUNDARY_IN_VALUE(token_type)) {
                continue;
            }
        }

        if (tokenizer_char_member_of(token_type, c)) {
            token_value[token_value_len] = c;
            token_value_len++;
            if (token_value_len >= buffer_len) {
                buffer_len *= 2;
                token_value = realloc(token_value, buffer_len);
                if (token_value == NULL) {
                    fprintf(stderr, "Error tokenizing source code. Out of memory.\n");
                    return NULL;
                }
            }
        } else if (tokenizer_char_is_terminal(token_type, c)) {
            break;
        } else {
            fseek(src, -1, SEEK_CUR);
            break;
        }
    }

    token_value[token_value_len] = 0;

    if (token_type == TK_UNSET) {
        free(token_value);
        return NULL;
    }

    TOKEN *token = malloc(sizeof(TOKEN));
    token->type = token_type;
    token->value = token_value;
    return token;
}

LINKED_LIST_ITEM* tokenizer_read_src(const char *src_path)
{
    FILE *src_file = fopen(src_path, "r");

    LINKED_LIST_ITEM *token_list_head = NULL;
    LINKED_LIST_ITEM *prev_token_item = NULL;
    LINKED_LIST_ITEM *token_item = NULL;
    TOKEN *token = NULL;
    while ((token = tokenizer_read_token(src_file)) != NULL)
    {
        if (TOKEN_LACKS_SEMANTIC_MEANING(token->type)) {
            tokenizer_free_token(token);
            continue;
        }

        prev_token_item = token_item;
        token_item = malloc(sizeof(LINKED_LIST_ITEM));
        token_item->value = (void *) token;

        if (prev_token_item == NULL) {
            token_list_head = token_item;
        } else {
            linked_list_insert_after(prev_token_item, token_item);
        }
    }
    fclose(src_file);
    return token_list_head;
}

TOKEN_PARSER* tokenizer_init_parser(const char *src)
{
    TOKEN_PARSER *parser = malloc(sizeof(TOKEN_PARSER));
    parser->token_list = tokenizer_read_src(src);
    parser->token_cur = parser->token_list;
    return parser;
}