#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "./lexer.h"
#include "./rules.h"
#include "./debug.h"

void PRINT_LEXER_ERROR(LEXER *parser, const char *message) {
    if (parser) {
        fprintf(
            stderr,
            "LEXER :: %s @ %s#L%d:%d\n",
            message,
            parser->current_file_path,
            parser->current_lineno,
            parser->current_colno
        );
    } else {
        fprintf(stderr, "LEXER :: %s\n", message);
    }
}

void lexer_free_token(TOKEN *token)
{
    if (token->buffer != NULL) free(token->buffer);
    free(token);
}

void lexer_populate_char_buffer(LEXER *parser) {
    if (parser->char_buffer_n < LEXER_CHAR_BUFFER_SIZE) {
        /* Fill out buffer if possible. */
        char *empty_buffer_offset = parser->char_buffer + (uint8_t) parser->char_buffer_n;
        int space_available = LEXER_CHAR_BUFFER_SIZE - parser->char_buffer_n;
        int chars_read = fread(empty_buffer_offset, sizeof(char), space_available, parser->current_file);
        parser->char_buffer_n += chars_read;
    }
}

int lexer_read_char_buffer(LEXER *parser, char *c, char *cc) {
    lexer_populate_char_buffer(parser);

    if (!parser->char_buffer_n) return 0;

    *c = 0;
    *cc = 0;

    if (parser->char_buffer_n) *c = parser->char_buffer[0];
    if (parser->char_buffer_n > 1) *cc = parser->char_buffer[1];

    return 1;
}

TOKEN_RULE *find_rule_by_open_chars(
    LEXER *parser,
    char c,
    char cc,
    int *chars_consumed
) {
    TOKEN_RULE *token_rule = parser->token_rules;
    while (token_rule != NULL) {
        if (token_rule->open_chars == NULL) {
            ASSERT_LEXER_ERROR("Bad token rule. Missing rule charset.");
        }

        if (matches_token_rule_charset(token_rule->open_chars, c, cc, chars_consumed)) {
            return token_rule;
        }

        token_rule = token_rule->next;
    }

    return NULL;
}

void lexer_left_shift_char_buffer(LEXER *parser, int n) {
    if (!n) return;

    int i;
    int pos_to_copy;
    for (i = 0; i < parser->char_buffer_n; i++) {
        pos_to_copy = i + n;
        if (pos_to_copy >= parser->char_buffer_n) {
            parser->char_buffer[i] = 0;
        } else {
            parser->char_buffer[i] = parser->char_buffer[pos_to_copy];
        }
    }

    if (n > parser->char_buffer_n) {
        parser->char_buffer_n = 0;
    } else {
        parser->char_buffer_n = parser->char_buffer_n - n;
    }
}

void lexer_consume_char(LEXER *parser, int n) {
    if (!n) return;

    int i;
    for (i = 0; i < n; i++) {
        char c = parser->char_buffer[i];
        parser->current_colno += 1;
        if (c == '\n') {
            parser->current_lineno += 1;
            parser->current_colno = 0;
        }
    }

    lexer_left_shift_char_buffer(parser, n);
}

TOKEN *lexer_allocate_token(LEXER *parser, TOKEN_TYPE type) {
    TOKEN *token = malloc(sizeof(TOKEN));
    token->type = type;
    token->lineno = parser->current_lineno;
    token->colno = parser->current_colno;
    token->file_path = parser->current_file_path;
    token->buffer_len = 32;
    token->data_len = 0;
    token->buffer = (uint8_t *) malloc(sizeof(uint8_t) * token->buffer_len);
    token->buffer[0] = 0;
    return token;
}

TOKEN* lexer_read_token(LEXER *parser) {
    char c = 0;
    char cc = 0;
    if (!lexer_read_char_buffer(parser, &c, &cc)) return NULL;

    int chars_open = 0;
    TOKEN_RULE *token_rule = find_rule_by_open_chars(parser, c, cc, &chars_open);
    TOKEN *token = lexer_allocate_token(parser, token_rule->type);

    if (!token_rule->inclusive_boundary) {
        // skip open characters
        lexer_consume_char(parser, chars_open);
    }

    while (token_rule->member_chars != NULL && lexer_read_char_buffer(parser, &c, &cc)) {
        if (matches_token_rule_charset(token_rule->member_chars, c, cc, NULL)) {
            token->buffer[token->data_len++] = c;
            if (token->data_len >= token->buffer_len) {
                token->buffer_len *= 2;
                token->buffer = (uint8_t *) realloc(token->buffer, token->buffer_len);
                if (token->buffer == NULL) {
                    ASSERT_LEXER_ERROR("Error parsing source code. Out of memory.");
                }
            }
            token->buffer[token->data_len] = 0;
            lexer_consume_char(parser, 1);
            continue;
        }
        
        int chars_closed = 0;
        if (matches_token_rule_charset(token_rule->close_chars, c, cc, &chars_closed)) {
            lexer_consume_char(parser, chars_closed);
        }

        // don't understand char in context of
        // current rule, let next rule deal with it
        break;
    }

    return token;
}

int TOKEN_LACKS_SEMANTIC_MEANING(TOKEN_TYPE type) {
    return type == TK_BLOCK_COMMENT || type == TK_NEWLINE || type == TK_LINE_COMMENT;
}

void lexer_read_src(LEXER *parser, const char *src_path)
{
    parser->current_file = fopen(src_path, "r");

    parser->current_file_path = src_path;
    parser->current_lineno = 0;
    parser->current_colno = 0;

    TOKEN *token_list_head = NULL;
    TOKEN *prev_token = NULL;
    TOKEN *token = NULL;

    while ((token = lexer_read_token(parser)) != NULL)
    {
        //lexer_debug_token(token);

        if (TOKEN_LACKS_SEMANTIC_MEANING(token->type)) {
            lexer_free_token(token);
            continue;
        }

        if (prev_token != NULL) {
            token->prev = prev_token;
            prev_token->next = token;
        } else {
            token_list_head = token;
        }
        prev_token = token;
    }
    fclose(parser->current_file);

    parser->token_list = token_list_head;
    parser->token_cur = parser->token_list;
}

LEXER* lexer_init_parser(const char *src)
{
    LEXER *parser = malloc(sizeof(LEXER));
    parser->char_buffer = malloc(sizeof(char) * LEXER_CHAR_BUFFER_SIZE);
    parser->char_buffer_n = 0;
    parser->current_file_path = NULL;
    parser->current_lineno = 0;
    parser->current_colno = 0;
    parser->token_rules = build_token_rules();
    lexer_read_src(parser, src);
    return parser;
}