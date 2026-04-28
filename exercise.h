#ifndef EXERCISE_H
#define EXERCISE_H

#include <stdbool.h>

typedef struct Form Form;
typedef struct CurrentRoutine CurrentRoutine;

typedef struct Exercise {
    char* id;
    char* title;
    int reps;
    int sets;
} Exercise;

typedef struct CurrentExercise {
    char* id;
    char* title;
    int* reps;
    int sets;
    bool done;
} CurrentExercise;

void init_current_exercises(CurrentExercise* current, Exercise* exercises);
void exercise_arr_add(Exercise** exercises_ptr, Form form);

CurrentExercise** get_exercise_history(CurrentRoutine** history_ptr, char* id);

void free_exercises(Exercise** exercises_ptr);
void free_history_exercises(CurrentExercise** history_ptr);

void generate_exercise_id(Exercise* exercise);

#endif
