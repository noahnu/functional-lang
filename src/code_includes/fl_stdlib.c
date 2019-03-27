#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "fl_directives.h"

typedef enum {
    DT_STRING,
    DT_INT64,
    DT_BOOL,
    DT_DECIMAL
} _FL_DATA_TYPE;

typedef struct {
    char            *key;
    _FL_DATA_TYPE   data_type;
    uint32_t        value;
} _FL_SYMBOL_TABLE;

typedef struct {
    _FL_SYMBOL_TABLE    *symbols;
} _FL_CLOSURE_STRUCT;

typedef struct {
    
} _FL_OBJECT_STRUCT;