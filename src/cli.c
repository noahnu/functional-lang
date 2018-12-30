#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "./ast.h"
#include "./ast_debug.h"


int usage(const char *program)
{
    printf("%s <filename>\n", program);
    return EXIT_FAILURE;
}

int main(int argc, const char *argv[])
{
    if (argc <= 1) return usage(argv[0]);

    const char* input_path = argv[1];
    AST_NODE *ast_root = ast_parse_file(input_path);

    AST_DEBUG_OPTS ast_debug_options = {
        .include_closure_ptrs = true,
        .include_token_location = false,
        .include_node_value = true
    };

    ast_debug_graph(ast_root, &ast_debug_options);

    return EXIT_SUCCESS;
}
