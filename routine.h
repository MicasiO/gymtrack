#ifndef ROUTINE_H
#define ROUTINE_H

#include <stdbool.h>
#include "exercise.h"
#include "stdlib.h"

typedef struct Form Form;

typedef struct Routine {
    char* title;
    Exercise* exercises;
    time_t last_done;
    char* id;
} Routine;

typedef struct CurrentRoutine {
    char* title;
    char* id;
    CurrentExercise* exercises;
    time_t last_done;
    long long index;
} CurrentRoutine;

void routine_arr_add(Routine** routines_ptr, Routine* routine);
void routine_arr_remove(Routine** routines_ptr, char* id);
void generate_routine_id(Routine* routines, Routine* routine);

void update_routine_last_done(Routine** routines_ptr, char* id);
time_t get_routine_last_done(Routine** routines_ptr, char* id);

CurrentRoutine* get_last_routine(CurrentRoutine* history, char* id);
CurrentRoutine* init_current_routine(Routine* routine);

bool is_routine_done(CurrentRoutine* routine);

void free_draft_routine(Routine* draft);
void free_current_routine(CurrentRoutine* current);
void free_routines(Routine* routines);
void free_history(CurrentRoutine* history);

#endif
