#ifndef EXERCISE_H
#define EXERCISE_H

#include <stdbool.h>

typedef struct Form Form;

typedef struct Exercise {
    char* title;
    int reps;
    int sets;
} Exercise;

typedef struct CurrentExercise {
    char* title;
    int* reps;
    int sets;
    bool done;
} CurrentExercise;

void init_current_exercises(CurrentExercise* current, Exercise* exercises);
void exercise_arr_add(Exercise** exercises, Form form);

#endif
