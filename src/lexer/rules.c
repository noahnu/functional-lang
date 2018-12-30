#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "./rules.h"
#include "./token_types.h"
#include "./charsets.h"

TOKEN_RULE *define_token_rule(
    TOKEN_TYPE type,
    TOKEN_RULE_CHARSET *open_chars,
    TOKEN_RULE_CHARSET *member_chars,
    TOKEN_RULE_CHARSET *close_chars,
    int inclusive_boundary
) {
    assert(open_chars != NULL);

    TOKEN_RULE *rule = malloc(sizeof(TOKEN_RULE));
    rule->type = type;
    rule->open_chars = open_chars;
    rule->member_chars = member_chars;
    rule->close_chars = close_chars;
    rule->next = NULL;
    rule->prev = NULL;
    rule->inclusive_boundary = inclusive_boundary;
    return rule;
}

TOKEN_RULE_CHARSET *define_token_rule_charset(
    const char **literals,
    int (**matchers)(char, char)
) {
    TOKEN_RULE_CHARSET *charset = malloc(sizeof(TOKEN_RULE_CHARSET));
    charset->literals = NULL;
    charset->matchers = NULL;

    if (literals != NULL) {
        int literals_n = 0;
        while ((literals[literals_n]) != NULL) literals_n += 1;
        charset->literals = (const char **) malloc(sizeof(char *) * (literals_n + 1));
        for (int i = 0; i < literals_n; i++) {
            charset->literals[i] = strdup(literals[i]);
        }
        charset->literals[literals_n] = NULL;
    }

    if (matchers != NULL) {
        int matchers_n = 0;
        while ((matchers[matchers_n]) != NULL) matchers_n += 1;
        charset->matchers = (int (**)(char, char)) malloc(sizeof(int (*)(char, char)) * (matchers_n + 1));
        for (int i = 0; i < matchers_n; i++) {
            charset->matchers[i] = matchers[i];
        }
        charset->matchers[matchers_n] = NULL;
    }

    return charset;
}

int matches_token_rule_charset(
    TOKEN_RULE_CHARSET *charset,
    char c,
    char cc,
    int *chars_consumed
) {
    if (chars_consumed != NULL) *chars_consumed = 0;

    if (charset == NULL) {
        // Don't match anything.
        return 0;
    }

    if (charset->literals != NULL) {
        int literal_pos = 0;
        const char *literal = NULL;
        while (charset->literals[literal_pos] != NULL) {
            literal = charset->literals[literal_pos];
            literal_pos += 1;
            if (literal[0] != c) continue;
            if (literal[1] != 0 && literal[1] != cc) continue;

            if (chars_consumed != NULL) {
                *chars_consumed = 1;
                if (literal[1] != 0) {
                    *chars_consumed = 2;
                }
            }

            return 1;
        }
    }
    if (charset->matchers != NULL) {
        int matcher_pos = 0;
        int (*matcher)(char, char) = NULL;
        while (charset->matchers[matcher_pos] != NULL) {
            matcher = charset->matchers[matcher_pos];
            matcher_pos += 1;
            int chars = 0;
            if ((chars = matcher(c, cc))) {
                if (chars_consumed) *chars_consumed = chars;
                return 1;
            }
        }
    }
    return 0;
}

void register_token_rule(TOKEN_RULE **token_rules, TOKEN_RULE *new_rule) {
    if (*token_rules == NULL) {
        *token_rules = new_rule;
    } else {
        new_rule->next = *token_rules;
        new_rule->prev = NULL;
        (*token_rules)->prev = new_rule;
        *token_rules = new_rule;
    }
}

TOKEN_RULE* build_token_rules() {
    TOKEN_RULE_CHARSET *TK_RULE_CHARSET_NEWLINE = define_token_rule_charset(
        NULL,
        (int (*[])(char, char)) { CHARSET_NEWLINE, NULL }
    );

    TOKEN_RULE_CHARSET *TK_RULE_CHARSET_WHITESPACE = define_token_rule_charset(
        NULL,
        (int (*[])(char, char)) { CHARSET_NEWLINE, CHARSET_WHITESPACE, NULL }
    );

    TOKEN_RULE *token_rules = NULL;

    register_token_rule(&token_rules, define_token_rule(
        TK_NUMERIC,
        define_token_rule_charset(
            NULL,
            (int (*[])(char, char)) { CHARSET_DIGIT, NULL }),
        define_token_rule_charset(
            (const char *[]) { ".", 0 },
            (int (*[])(char, char)) { CHARSET_DIGIT, NULL }),
        NULL,
        1
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_STRING,
        define_token_rule_charset((const char *[]) { "`", NULL }, NULL),
        define_token_rule_charset(
            NULL,
            (int (*[])(char, char)) { CHARSET_STRING, NULL }),
        define_token_rule_charset((const char *[]) { "`", NULL }, NULL),
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_NEWLINE,
        TK_RULE_CHARSET_WHITESPACE,
        TK_RULE_CHARSET_WHITESPACE,
        NULL,
        1
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_OPERATOR,
        define_token_rule_charset(
            NULL,
            (int (*[])(char, char)) { CHARSET_OPERATOR, NULL }),
        define_token_rule_charset(
            NULL,
            (int (*[])(char, char)) { CHARSET_OPERATOR, NULL }),
        NULL,
        1
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_IDENTIFIER,
        define_token_rule_charset(
            (const char *[]) { "_", "$", NULL },
            (int (*[])(char, char)) { CHARSET_ALPHA, NULL }),
        define_token_rule_charset(
            (const char *[]) { "_", "$", NULL },
            (int (*[])(char, char)) {
                CHARSET_ALPHA,
                CHARSET_DIGIT,
                NULL
            }
        ),
        NULL,
        1
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_PAREN_CLOSE,
        define_token_rule_charset((const char *[]) { ")", NULL }, NULL),
        NULL,
        NULL,
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_PAREN_OPEN,
        define_token_rule_charset((const char *[]) { "(", NULL }, NULL),
        NULL,
        NULL,
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_PARAM_CLOSE,
        define_token_rule_charset((const char *[]) { "}", NULL }, NULL),
        NULL,
        NULL,
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_PARAM_OPEN,
        define_token_rule_charset((const char *[]) { "{", NULL }, NULL),
        NULL,
        NULL,
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_CLOSURE,
        define_token_rule_charset((const char *[]) { "->", "=>", NULL }, NULL),
        NULL,
        NULL,
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_LINE_COMMENT,
        define_token_rule_charset((const char *[]) { "#", NULL }, NULL),
        define_token_rule_charset(
            NULL,
            (int (*[])(char, char)) { CHARSET_LINE_COMMENT, NULL }),
        TK_RULE_CHARSET_NEWLINE,
        0
    ));

    register_token_rule(&token_rules, define_token_rule(
        TK_BLOCK_COMMENT,
        define_token_rule_charset((const char *[]) { "/*", NULL }, NULL),
        define_token_rule_charset(
            NULL,
            (int (*[])(char, char)) { CHARSET_BLOCK_COMMENT, NULL }),
        define_token_rule_charset((const char *[]) { "*/", NULL }, NULL),
        0
    ));

    return token_rules;
}
