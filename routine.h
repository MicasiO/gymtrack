#ifndef ROUTINE_H
#define ROUTINE_H

#include "exercise.h"
#include "stdlib.h"

typedef struct Form Form;

typedef struct Routine {
    char* title;
    Exercise* exercises;
    time_t last_done;
    long long id;
} Routine;

void routine_arr_add(Routine** routines_ptr, Routine routine);
void routine_arr_remove(Routine** routines_ptr, Routine* routine);
void exercise_arr_add(Exercise** exercises, Form form);
int generate_routine_id(Routine* routines);

#endif
