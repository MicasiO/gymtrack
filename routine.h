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

typedef struct CurrentRoutine {
    char* title;
    long long routine_id;
    CurrentExercise* exercises;
    time_t date;
    long long index;
} CurrentRoutine;

void routine_arr_add(Routine** routines_ptr, Routine* routine);
void routine_arr_remove(Routine** routines_ptr, long long id);
void exercise_arr_add(Exercise** exercises, Form form);
int generate_routine_id(Routine* routines);
CurrentRoutine* get_last_routine(CurrentRoutine* history, long long id);

CurrentRoutine* init_current_routine(Routine* routine);

void free_draft_routine(Routine* draft);
void free_current_routine(CurrentRoutine* current);
void free_routines(Routine* routines);
void free_history(CurrentRoutine* history);

#endif
