#include "exercise.h"
#include "form.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

void exercise_arr_add(Exercise** exercises_ptr, Form form) {
    Exercise* exercises = *exercises_ptr;
    Exercise ex;
    ex.title = get_field_value(&form.fields[0]);
    generate_exercise_id(&ex);
    char* sets_str = get_field_value(&form.fields[1]);
    char* reps_str = get_field_value(&form.fields[2]);

    ex.sets = atoi(sets_str) <= 0 ? 1 : atoi(sets_str);
    ex.reps = atoi(reps_str) <= 0 ? 1 : atoi(reps_str);

    free(sets_str);
    free(reps_str);

    arrput(exercises, ex);

    *exercises_ptr = exercises;
}

CurrentExercise** get_exercise_history(CurrentRoutine** history_ptr, char* id) {
    CurrentRoutine* history = *history_ptr;

    if (history == NULL || id == NULL) {
        return NULL;
    }

    CurrentExercise** exercise_hist = NULL;

    for (int i = arrlen(history) - 1; i >= 0; i--) {
        for (int j = arrlen(history[i].exercises) - 1; j >= 0; j--) {
            if (history[i].exercises[j].done != true) {
                continue;
            }

            if (strcmp(history[i].exercises[j].id, id) == 0) {
                arrput(exercise_hist, &history[i].exercises[j]);
            }
        }
    }

    return exercise_hist;
}

void generate_exercise_id(Exercise* exercise) {
    char uuid[37];
    generate_uuid(uuid);
    exercise->id = strdup(uuid);
}

void free_history_exercises(CurrentExercise** history_ptr) {
    CurrentExercise* history = *history_ptr;

    if (history == NULL) {
        return;
    }

    for (int i = 0; i < arrlen(history); i++) {
        if (history[i].title != NULL) {
            free(history[i].title);
        }

        if (history[i].id != NULL) {
            free(history[i].id);
        }

        arrfree(history[i].reps);
    }

    arrfree(history);
}

void free_exercises(Exercise** exercises_ptr) {
    Exercise* exercises = *exercises_ptr;

    if (exercises == NULL) {
        return;
    }

    for (int i = 0; i < arrlen(exercises); i++) {
        if (exercises[i].title != NULL) {
            free(exercises[i].title);
        }
        if (exercises[i].id != NULL) {
            free(exercises[i].id);
        }
    }

    arrfree(exercises);
}
