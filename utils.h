#ifndef UTILS_H
#define UTILS_H

#include "routine.h"

void die(const char* s);
int is_str_empty(char* str);
void serialize(Routine* routines);

#endif
