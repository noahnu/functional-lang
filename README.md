## Grammar

```
Letter :: <A - Z> | <a - z>
Digit :: <0 - 9>

Identifier :: (<Letter> | "_") [<Digit>] [<Operator>] [<Identifier>]

Operator :: "+" | "-" | "/" | "*"

Expression ::
    | <Numeric>
    | <String>
    | "(" <Expression> ")"
    | <Identifier>
    | <Call>
    | <Function Definition>

Call ::
    | <Expression> <Expression> ...

Param List ::
    | "{" <Identifier> ... "}"

Closure ::
    | <Identifier> [<Param List>] "->" <Expression> 

```

## Tokenization

### Token Types

```
    TK_PAREN_OPEN,
    TK_PAREN_CLOSE,
    TK_OBJECT_OPEN,
    TK_OBJECT_CLOSE,
    TK_IDENTIFIER,
    TK_NUMERIC,
    TK_CLOSURE,
    TK_NEWLINE,
    TK_STRING,
    TK_BLOCK_COMMENT,
    TK_LINE_COMMENT,
    TK_UNSET,
```

## Abstract Syntax Tree

### AST Node Types

```
    AST_T_EXPRESSION,
    AST_T_NUMERIC,
    AST_T_STRING,
    AST_T_CALL,
    AST_T_IDENTIFIER,
```

All nodes must be a descendant of an AST_T_EXPRESSION node.

### Visualization of AST

```
make ast-graphs
```

### Examples

```
# Test of nested expressions.
(
    /* This is a block comment
     * spanning multiple lines. */
    add 1 2 (multiply 2 5) 3
)
```

```
/* Creates a function "double" and immediately
 * calls it with the argument x=5. Should print 10. */
(print (
    (double { x } -> (multiple x 2))
    5
))
```