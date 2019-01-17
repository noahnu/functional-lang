#include <stdint.h>
#include <stdbool.h>

#ifndef _H_CODE_GENERATOR_
#define _H_CODE_GENERATOR_

typedef struct _source_code_opts {
    bool dummy;
} SOURCE_CODE_OPTS;

typedef struct _source_code {
    FILE *output_file;
} CG_SOURCE_CODE;

typedef enum {
    DT_STRING,
    DT_INT64,
    DT_BOOL,
} CG_DATA_TYPE;

typedef struct _variable {
    const char      *identifier;
    CG_DATA_TYPE    data_type;
} CG_VARIABLE;

typedef enum {
    OP_BC_CALL, // <OP> <ClosureRef> <ArgListRef>
} CG_OP_BYTECODE;

typedef struct _statement {
    struct _statement   *prev;
    struct _statement   *next;
    CG_OP_BYTECODE  bytecode;
    // TODO: args..
} CG_STATEMENT;

typedef struct _closure_frame {
    struct _closure_frame   *parent;
    CG_VARIABLE             *variables;
    CG_STATEMENT            *body;
} CG_CLOSURE;

CG_CLOSURE *cg_init_closure(AST_NODE *node_closure);

void generate_source_code(
    const char *output_path, 
    AST_NODE *ast_root,
    SOURCE_CODE_OPTS *opts
);

#endif
