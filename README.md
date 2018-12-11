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
    | <Sequence>
    | <Infix_Operation>

Call ::
    | <Expression> <Expression>

Infix_Operation ::
    | <Expression> <Operator> <Expression>
    | <Numeric> <Operator> <Expression>
    | <String> <Operator> <Expression>

Sequence :: <Expression> ";" <Expression>

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
    AST_T_INFIX_OPERATION,
    AST_T_NUMERIC,
    AST_T_STRING,
    AST_T_CALL,
    AST_T_IDENTIFIER,
    AST_T_SEQUENCE,
```

All nodes must be a descendant of an AST_T_EXPRESSION node.

### Visualization of AST

```
make; ./bin/flang ./tests/test.fl | dot -Tsvg > graph.svg
```