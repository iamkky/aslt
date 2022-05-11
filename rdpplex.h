#ifndef _rdpplex_h_
#define _rdpplex_h_

enum TOKENS {COLON=1000, SEMICOLON, VBAR, TERMINAL, NONTERMINAL, CODEBETWEENCURLYBRACES};
static char *tokens[]= {"COLON", "SEMICOLON", "VBAR", "TERMINAL", "NONTERMINAL", "CODEBETWEENCURLYBRACES"};

int rdpplex(char *buffer, int *cursor, char* *value, void *extra);

#endif
