#ifndef _H_TOKEN_TYPES_
#define _H_TOKEN_TYPES_

typedef enum {
    TK_PAREN_OPEN, // groups expressions, enforcing ordering
    TK_PAREN_CLOSE,
    TK_OBJECT_OPEN, // groups function parameters when immediately
                   // followed by closure operator
    TK_OBJECT_CLOSE,
    TK_IDENTIFIER,
    TK_NUMERIC,
    TK_CLOSURE, // creates function closure
    TK_NEWLINE,
    TK_STRING,
    TK_BLOCK_COMMENT,
    TK_LINE_COMMENT,
    TK_UNSET,
} TOKEN_TYPE;

#endif