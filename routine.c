#include "routine.h"
#include <string.h>
#include <time.h>
#include "exercise.h"
#include "form.h"
#include "stb_ds.h"
#include "utils.h"

void routine_arr_add(Routine** routines_ptr, Routine* routine) {
    Routine* routines = *routines_ptr;
    routine->id = generate_routine_id(routines);
    arrput(routines, *routine);
    *routines_ptr = routines;
    serialize_routines(routines);
}

void routine_arr_remove(Routine** routines_ptr, long long id) {
    Routine* routines = *routines_ptr;
    for (int i = 0; i < arrlen(routines); i++) {
        if (routines[i].id == id) {
            free(routines[i].title);
            if (routines[i].exercises != NULL) {
                for (int j = 0; j < arrlen(routines[i].exercises); j++) {
                    free(routines[i].exercises[j].title);
                }
                arrfree(routines[i].exercises);
            }

            arrdel(routines, i);

            *routines_ptr = routines;
            break;
        }
    }
    serialize_routines(*routines_ptr);
}

void exercise_arr_add(Exercise** exercises, Form form) {
    Exercise ex;
    ex.title = get_field_value(&form.fields[0]);
    char* sets_str = get_field_value(&form.fields[1]);
    char* reps_str = get_field_value(&form.fields[2]);

    ex.sets = atoi(sets_str);
    ex.reps = atoi(reps_str);

    free(sets_str);
    free(reps_str);

    arrput(*exercises, ex);
}

int generate_routine_id(Routine* routines) {
    int highest_id = 0;

    for (int i = 0; i < arrlen(routines); i++) {
        if (routines[i].id > highest_id) {
            highest_id = routines[i].id;
        }
    }

    return highest_id + 1;
}

CurrentRoutine* get_last_routine(CurrentRoutine* routines_hist, long long id) {
    for (int i = arrlen(routines_hist) - 1; i >= 0; i--) {
        if (routines_hist[i].routine_id == id) {
            return &routines_hist[i];
        }
    }
    return NULL;
}

CurrentRoutine* init_current_routine(Routine* routine) {
    if (routine == NULL) {
        return NULL;
    }
    CurrentRoutine* current = (CurrentRoutine*)malloc(sizeof(CurrentRoutine));
    current->title = strdup(routine->title);
    current->routine_id = routine->id;
    current->date = time(NULL);
    current->exercises = NULL;
    current->index = 0;

    for (int i = 0; i < arrlen(routine->exercises); i++) {
        Exercise ex = routine->exercises[i];
        CurrentExercise current_ex;

        current_ex.title = strdup(ex.title);
        current_ex.sets = ex.sets;
        current_ex.reps = ex.reps;
        current_ex.done = false;

        arrput(current->exercises, current_ex);
    }

    return current;
}

void free_draft_routine(Routine* draft) {
    if (draft == NULL) {
        return;
    }
    free(draft->title);
    if (draft->exercises != NULL) {
        for (int i = 0; i < arrlen(draft->exercises); i++) {
            free(draft->exercises[i].title);
        }

        arrfree(draft->exercises);
    }

    free(draft);
}

void free_current_routine(CurrentRoutine* current) {
    if (current == NULL) {
        return;
    }

    free(current->title);

    if (current->exercises != NULL) {
        for (int i = 0; i < arrlen(current->exercises); i++) {
            free(current->exercises[i].title);
        }

        arrfree(current->exercises);
    }

    free(current);
}

void free_routines(Routine* routines) {
    if (routines == NULL) {
        return;
    }

    for (int i = 0; i < arrlen(routines); i++) {
        if (routines[i].title != NULL) {
            free(routines[i].title);
        }

        if (routines[i].exercises != NULL) {
            for (int j = 0; j < arrlen(routines[i].exercises); j++) {
                if (routines[i].exercises[j].title != NULL) {
                    free(routines[i].exercises[j].title);
                }
            }
            arrfree(routines[i].exercises);
        }
    }

    arrfree(routines);
}

void free_history(CurrentRoutine* history) {
    if (history == NULL) {
        return;
    }

    for (int i = 0; i < arrlen(history); i++) {
        if (history[i].title != NULL) {
            free(history[i].title);
        }

        if (history[i].exercises != NULL) {
            for (int j = 0; j < arrlen(history[i].exercises); j++) {
                if (history[i].exercises[j].title != NULL) {
                    free(history[i].exercises[j].title);
                }
            }
            arrfree(history[i].exercises);
        }
    }

    arrfree(history);
}

void curr_routine_arr_add(CurrentRoutine** routines_ptr, CurrentRoutine* routine) {
    CurrentRoutine* routines = *routines_ptr;
    arrput(routines, *routine);
    *routines_ptr = routines;
    serialize_history(routines);
}
