#include "./token_types.h"

#ifndef _H_RULES_
#define _H_RULES_

typedef struct _token_rule_charset {
    const char    **literals;
    int           (**matchers)(char, char);
} TOKEN_RULE_CHARSET;

typedef struct _token_rule {
    TOKEN_TYPE              type;
    TOKEN_RULE_CHARSET      *open_chars;
    TOKEN_RULE_CHARSET      *member_chars;
    TOKEN_RULE_CHARSET      *close_chars;
    int                     inclusive_boundary;
    struct _token_rule      *prev;
    struct _token_rule      *next;
} TOKEN_RULE;

TOKEN_RULE *define_token_rule(
    TOKEN_TYPE type,
    TOKEN_RULE_CHARSET *open_chars,
    TOKEN_RULE_CHARSET *member_chars,
    TOKEN_RULE_CHARSET *close_chars,
    int inclusive_boundary
);

TOKEN_RULE_CHARSET *define_token_rule_charset(
    const char **literals,
    int (**matchers)(char, char)
);

int matches_token_rule_charset(
    TOKEN_RULE_CHARSET *charset,
    char c,
    char cc,
    int *chars_consumed
);

void register_token_rule(TOKEN_RULE **token_rules, TOKEN_RULE *new_rule);

TOKEN_RULE* build_token_rules();

#endif
