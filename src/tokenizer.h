
#include <stdint.h>
#include "./data_structures.h"

#ifndef _H_TOKENIZER_
#define _H_TOKENIZER_

typedef enum {
    TK_PAREN_OPEN,
    TK_PAREN_CLOSE,
    TK_IDENTIFIER,
    TK_NUMERIC,
    TK_OPERATOR,
    TK_NEWLINE,
    TK_STRING,
    TK_BLOCK_COMMENT,
    TK_LINE_COMMENT,
    TK_UNSET,
} TOKEN_TYPE;

typedef struct _token {
    TOKEN_TYPE  type;
    uint8_t     *value;
} TOKEN;

int CHAR_IS_LETTER(char c);
int CHAR_IS_DIGIT(char c);
int CHAR_IS_NUMERIC_SEPARATOR(char c);
int CHAR_IS_VALID_IDENTIFIER_SYMBOL(char c);
int CHAR_IS_VALID_IDENTIFIER_ENTITY(char c);
int CHAR_IS_NEWLINE(char c);
int CHAR_IS_WHITESPACE(char c);
int CHAR_IS_STRING_DELIMITER(char c);
int CHAR_IS_PAREN_OPEN(char c);
int CHAR_IS_PAREN_CLOSE(char c);
int CHAR_IS_OPERATOR(char c);
int INCLUDE_TOKEN_BOUNDARY_IN_VALUE(TOKEN_TYPE type);
int TOKEN_TYPE_SUPPORTS_VALUE(TOKEN_TYPE type);

int TOKEN_LACKS_SEMANTIC_MEANING(TOKEN_TYPE token_type);

TOKEN_TYPE tokenizer_identify_by_leading_char(uint8_t c);
int tokenizer_char_member_of(TOKEN_TYPE token_type, uint8_t c);
int tokenizer_char_is_terminal(TOKEN_TYPE token_type, uint8_t c);

void tokenizer_free_token(TOKEN *token);
char *tokenizer_token_type_to_name(TOKEN_TYPE type);

char *tokenizer_token_value_to_str(TOKEN_TYPE type, uint8_t *value);
void tokenizer_debug_token(TOKEN *token);

typedef struct _tokenizer_parser {
    LINKED_LIST_ITEM    *token_list;
    LINKED_LIST_ITEM    *token_cur;
} TOKEN_PARSER;

TOKEN_PARSER* tokenizer_init_parser(const char *src);

TOKEN* tokenizer_read_token(FILE *src);
LINKED_LIST_ITEM* tokenizer_read_src(const char *src_path);

#endif