#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "./ast.h"


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

    ast_debug_graph(ast_root);

    return EXIT_SUCCESS;
}
