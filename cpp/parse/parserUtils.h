#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

int getNextChar(char *b, int maxBuffer);
void beginToken(const char *t);
void printError(const char *errorstring, ...);
void parseComplete();

#endif //PARSER_UTILS_H
