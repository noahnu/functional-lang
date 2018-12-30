// returns number of characters consumed to match rule.

int CHARSET_ALPHA(char c, char cc) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        return 1;
    }
    return 0;
}

int CHARSET_DIGIT(char c, char cc) {
    if (c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

int CHARSET_OPERATOR(char c, char cc) {
    if (
        c == '>' || c == '<' || c == '+' ||
        c == '-' || c == '*' || c == '/'
    ) {
        return 1;
    }
    return 0;
}

int CHARSET_STRING(char c, char cc) {
    return c != '`';
}

int CHARSET_BLOCK_COMMENT(char c, char cc) {
    return c != '*' || cc != '/';
}

int CHARSET_NEWLINE(char c, char cc) {
    return c == '\n' || c == '\r';
}

int CHARSET_WHITESPACE(char c, char cc) {
    return c == ' ' || c == '\t';
}

int CHARSET_LINE_COMMENT(char c, char cc) {
    return !CHARSET_NEWLINE(c, cc);
}

int CHARSET_ANY(char c, char cc) {
    return 1;
}
