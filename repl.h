#include "stdlib.h"
#include "main.h"

char *readline(const char *prompt);
void add_history(const char *line);
int eval(const char *str, te_variable* elements);
void repl(te_variable* elements);
