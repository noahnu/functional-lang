#include <stdio.h>

#include "./token_types.h"
#include "./rules.h"
#include "./lexer.h"

#ifndef _H_LEXER_DEBUG_
#define _H_LEXER_DEBUG_

char *lexer_token_type_to_name(TOKEN_TYPE type);
char *lexer_token_value_to_str(TOKEN_TYPE type, uint8_t *value);
void lexer_debug_token(TOKEN *token);

#endif