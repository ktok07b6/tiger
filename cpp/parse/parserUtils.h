#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

int getNextChar(char *b, int maxBuffer);
void beginToken(char *t);
void printError(char *errorstring, ...);
void parseComplete();

#endif //PARSER_UTILS_H
