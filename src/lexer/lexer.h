#include "./token_types.h"
#include "./rules.h"

#ifndef _H_LEXER_
#define _H_LEXER_

typedef struct _token {
    TOKEN_TYPE      type;
    const char      *file_path;
    uint32_t        lineno;
    uint32_t        colno;
    uint8_t         *buffer;
    uint16_t        buffer_len;
    uint16_t        data_len;
    struct _token   *prev;
    struct _token   *next;
} TOKEN;

#define LEXER_CHAR_BUFFER_SIZE 2

typedef struct _lexer_parser {
    TOKEN    *token_list;
    TOKEN    *token_cur;

    TOKEN_RULE  *token_rules;
    char        *char_buffer;
    uint8_t      char_buffer_n; // len of valid portion of buffer
    const char  *current_file_path;
    FILE        *current_file;
    uint32_t    current_lineno;
    uint32_t    current_colno;
} LEXER;

#define ASSERT_LEXER_ERROR(str) assert("LEXER :: " #str)
void PRINT_LEXER_ERROR(LEXER *parser, const char *message);

void lexer_free_token(TOKEN *token);
void lexer_populate_char_buffer(LEXER *parser);
int lexer_read_char_buffer(LEXER *parser, char *c, char *cc);
TOKEN_RULE *find_rule_by_open_chars(
    LEXER *parser,
    char c,
    char cc,
    int *chars_consumed
);
void lexer_left_shift_char_buffer(LEXER *parser, int n);
void lexer_consume_char(LEXER *parser, int n);
int TOKEN_LACKS_SEMANTIC_MEANING(TOKEN_TYPE type);
TOKEN *lexer_allocate_token(LEXER *parser, TOKEN_TYPE type);

TOKEN* lexer_read_token(LEXER *parser);
void lexer_read_src(LEXER *parser, const char *src_path);

LEXER* lexer_init_parser(const char *src);

#endif
