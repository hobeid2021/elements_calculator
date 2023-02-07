#include "repl.h"

#define USE_READLINE
#ifdef USE_READLINE 
#include <readline/readline.h>
#include <readline/history.h>
#else
char *readline(const char *prompt) {
    fprintf(stderr, "%s", prompt);
    char buf[1024];
    char *line = fgets(buf, sizeof(buf), stdin);
    if (line == NULL && feof(stdin)) {
        return NULL;
    } else if (line == NULL) {
        perror("fgets");
        return NULL;
    }

    size_t len = strlen(line);

    if (len < 1)
        return NULL;

    if (line[len - 1] == '\n') {
        line[len - 1] = '\0';
        len -= 1;
    }

    line = malloc(len + 1);
    strcpy(line, buf);
    return line;
}

#endif
void add_history(const char *line) {}

int eval(const char *str, te_variable* elements) {
    int err = 0;
    te_expr *expr = te_compile(str, elements, 121, &err);
    double r = te_eval(expr);
    //double r = te_interp(str, &err);
    if (err != 0) {
        printf("Error at position %i\n", err);
        return -1;
    } else {
        printf("%g\n", r);
        return 0;
    }
    te_free(expr);
}

void repl(te_variable *elements) {
    while (1) {
        char *line = readline("> ");
        if (line == NULL) {
            break;
        } else if (strcmp(line, "q") == 0 || strcmp(line, "quit") == 0 || strcmp(line,"exit") == 0) {
            free(line);
            break;
        }

        if (eval(line, elements) != -1) {
            add_history(line);
        }

        free(line);
    }
}
