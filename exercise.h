#ifndef EXERCISE_H
#define EXERCISE_H

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

#endif
