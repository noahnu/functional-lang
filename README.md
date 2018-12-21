## Grammar

```
Letter :: <A - Z> | <a - z>
Digit :: <0 - 9>

Identifier :: (<Letter> | "_") [<Digit>] [<Identifier>]

Operator :: "+" | "-" | "/" | "*" | "->"

Expression ::
    | <Numeric>
    | <String>
    | "(" <Expression> ")"
    | <Identifier>
    | <Call>
    | <Operator>

Call ::
    | <Expression> <Expression> ...
    | <Composition>

Composition ::
    | <Identifier> . <Identifier>
    | <Call> . <Call>

```

## Tokenization

### Token Types

```
    TK_PAREN_OPEN,
    TK_PAREN_CLOSE,
    TK_IDENTIFIER,
    TK_NUMERIC,
    TK_OPERATOR,
    TK_NEWLINE,
    TK_STRING,
    TK_BLOCK_COMMENT,
    TK_LINE_COMMENT,
    TK_SEQUENCE,
    TK_UNSET,
```

## Abstract Syntax Tree

### AST Node Types

```
    AST_T_EXPRESSION,
    AST_T_OPERATOR,
    AST_T_NUMERIC,
    AST_T_STRING,
    AST_T_CALL,
    AST_T_IDENTIFIER,
```

All nodes must be a descendant of an AST_T_EXPRESSION node.

### Visualization of AST

```
make; ./bin/flang ./tests/test.fl | dot -Tsvg > graph.svg
```

### Examples

```
(add 1 (multiply 2 5) 3)
```