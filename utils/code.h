#ifndef CODE_H
#define CODE_H

#include <string>

std::string getPathFromHome();

char* commandGenerator(const char* text, int state);

char** autocomplete(const char* text, int start, int end);

#endif