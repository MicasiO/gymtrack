#ifndef ROUTINE_H
#define ROUTINE_H

#include "exercise.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct {
    char* title;
    Exercise* exercises;
    time_t last_done;
} Routine;

#endif
