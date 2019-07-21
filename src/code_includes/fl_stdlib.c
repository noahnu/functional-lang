#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    DT_UNKNOWN,
    DT_STRING,
    DT_INT64,
    DT_BOOL,
    DT_DECIMAL
} _FL_DATA_TYPE;

typedef struct {
    _FL_DATA_TYPE   data_type;
    uint64_t        value;
} _FL_DATUM;

typedef struct {
    char            *key;
    _FL_DATUM       datum;
} _FL_SYMBOL_TABLE;

typedef struct {
    _FL_SYMBOL_TABLE    *symbols;
} _FL_CLOSURE_STRUCT;

typedef struct {
    _FL_DATUM datum;
} _FL_RESULT_STRUCT;

_FL_CLOSURE_STRUCT* _FL_INIT_CLOSURE() {
    _FL_CLOSURE_STRUCT *closure = malloc(sizeof(_FL_CLOSURE_STRUCT));
    closure->symbols = NULL;
    return closure;
}

_FL_SYMBOL_TABLE* _FL_INIT_SYMBOL(
    const char* key, _FL_DATA_TYPE data_type, uint32_t value);

_FL_SYMBOL_TABLE* _FL_INIT_SYMBOL(
    const char* key, _FL_DATA_TYPE data_type, uint32_t value
) {
    _FL_SYMBOL_TABLE *symbol = malloc(sizeof(_FL_SYMBOL_TABLE));
    symbol->key = key;
    symbol->datum.data_type = data_type;
    symbol->datum.value = value;
    return symbol;
}