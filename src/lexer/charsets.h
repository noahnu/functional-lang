#ifndef _H_CHARSETS_
#define _H_CHARSETS_

int CHARSET_ALPHA(char c, char cc);
int CHARSET_DIGIT(char c, char cc);
int CHARSET_ANY(char c, char cc);
int CHARSET_OPERATOR(char c, char cc);
int CHARSET_STRING(char c, char cc);
int CHARSET_BLOCK_COMMENT(char c, char cc);
int CHARSET_NEWLINE(char c, char cc);
int CHARSET_WHITESPACE(char c, char cc);
int CHARSET_LINE_COMMENT(char c, char cc);

#endif
