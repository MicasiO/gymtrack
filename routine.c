#include "routine.h"
#include <string.h>
#include <time.h>
#include "exercise.h"
#include "form.h"
#include "stb_ds.h"
#include "utils.h"

void routine_arr_add(Routine** routines_ptr, Routine* routine) {
    Routine* routines = *routines_ptr;
    generate_routine_id(routines, routine);
    arrput(routines, *routine);
    *routines_ptr = routines;
    serialize_routines(routines);
}

void routine_arr_remove(Routine** routines_ptr, char* id) {
    Routine* routines = *routines_ptr;
    for (int i = 0; i < arrlen(routines); i++) {
        if (strcmp(routines[i].id, id) == 0) {
            free(routines[i].title);
            free(routines[i].id);
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

void generate_routine_id(Routine* routines, Routine* routine) {
    char uuid[37];
    generate_uuid(uuid);
    routine->id = strdup(uuid);
}

CurrentRoutine* get_last_routine(CurrentRoutine* routines_hist, char* id) {
    for (int i = arrlen(routines_hist) - 1; i >= 0; i--) {
        if (strcmp(routines_hist[i].id, id) == 0) {
            return &routines_hist[i];
        }
    }
    return NULL;
}

void update_routine_last_done(Routine** routines_ptr, char* id) {
    if (id == NULL || routines_ptr == NULL) {
        return;
    }

    Routine* routines = *routines_ptr;

    for (int i = arrlen(routines) - 1; i >= 0; i--) {
        if (strcmp(routines[i].id, id) == 0) {
            routines[i].last_done = time(NULL);
            serialize_routines(routines);
            return;
        }
    }
}

time_t get_routine_last_done(Routine** routines_ptr, char* id) {
    if (id == NULL || routines_ptr == NULL) {
        return 0;
    }

    Routine* routines = *routines_ptr;

    for (int i = arrlen(routines) - 1; i >= 0; i--) {
        if (strcmp(routines[i].id, id) == 0) {
            return routines[i].last_done;
        }
    }

    return 0;
}

CurrentRoutine* init_current_routine(Routine* routine) {
    if (routine == NULL) {
        return NULL;
    }

    CurrentRoutine* current = (CurrentRoutine*)malloc(sizeof(CurrentRoutine));
    current->title = strdup(routine->title);
    current->id = strdup(routine->id);
    current->last_done = time(NULL);
    current->exercises = NULL;
    current->index = 0;

    for (int i = 0; i < arrlen(routine->exercises); i++) {
        Exercise ex = routine->exercises[i];
        CurrentExercise current_ex;

        current_ex.title = strdup(ex.title);
        current_ex.sets = ex.sets;
        current_ex.reps = NULL;
        for (int j = 0; j < ex.sets; j++) {
            arrput(current_ex.reps, ex.reps);
        }
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
    free(draft->id);

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
    free(current->id);

    if (current->exercises != NULL) {
        for (int i = 0; i < arrlen(current->exercises); i++) {
            free(current->exercises[i].title);
            arrfree(current->exercises[i].reps);
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

        if (routines[i].id != NULL) {
            free(routines[i].id);
        }

        if (routines[i].exercises != NULL) {
            for (int j = 0; j < arrlen(routines[i].exercises); j++) {
                if (routines[i].exercises[j].title != NULL) {
                    free(routines[i].exercises[j].title);
                }
                // arrfree(routines[i].exercises[j].reps);
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

        if (history[i].id != NULL) {
            free(history[i].id);
        }

        if (history[i].exercises != NULL) {
            for (int j = 0; j < arrlen(history[i].exercises); j++) {
                if (history[i].exercises[j].title != NULL) {
                    free(history[i].exercises[j].title);
                }
                arrfree(history[i].exercises[j].reps);
            }
            arrfree(history[i].exercises);
        }
    }

    arrfree(history);
}

bool is_routine_done(CurrentRoutine* routine) {
    for (int i = 0; i < arrlen(routine->exercises); i++) {
        if (routine->exercises[i].done == true) {
            return true;
        }
    }

    return false;
}
