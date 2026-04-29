#include "routine.h"
#include <string.h>
#include <time.h>
#include "exercise.h"
#include "serializer.h"
#include "stb_ds.h"
#include "utils.h"

void routine_arr_add(Routine** routines_ptr, Routine* routine) {
    Routine* routines = *routines_ptr;
    generate_routine_id(routine);
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
                    free(routines[i].exercises[j].id);
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

void generate_routine_id(Routine* routine) {
    char uuid[37];
    generate_uuid(uuid);
    routine->id = strdup(uuid);
}

CurrentRoutine* get_last_routine(CurrentRoutine* history, char* id) {
    for (int i = arrlen(history) - 1; i >= 0; i--) {
        if (strcmp(history[i].id, id) == 0) {
            return &history[i];
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
    current->duration = 0;
    current->exercises = NULL;
    current->index = 0;

    for (int i = 0; i < arrlen(routine->exercises); i++) {
        Exercise ex = routine->exercises[i];
        CurrentExercise current_ex;

        current_ex.title = strdup(ex.title);
        current_ex.id = strdup(ex.id);

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

CurrentRoutine** get_routine_history(CurrentRoutine** history_ptr, char* id) {
    CurrentRoutine* history = *history_ptr;

    if (history == NULL || id == NULL) {
        return NULL;
    }

    CurrentRoutine** routine_hist = NULL;

    for (int i = arrlen(history) - 1; i >= 0; i--) {
        if (strcmp(history[i].id, id) == 0) {
            arrput(routine_hist, &history[i]);
        }
    }

    return routine_hist;
}

void history_remove_oldest(CurrentRoutine** history_ptr, char* id) {
    CurrentRoutine* history = *history_ptr;

    if (history == NULL || id == NULL) {
        return;
    }

    time_t oldest_time = 0;
    int oldest_idx = -1;

    for (int i = 0; i < arrlen(history); i++) {
        if (strcmp(history[i].id, id) == 0) {
            if (oldest_idx == -1 || history[i].last_done < oldest_time) {
                oldest_idx = i;
                oldest_time = history[i].last_done;
            }
        }
    }

    if (oldest_idx != -1) {
        free(history[oldest_idx].title);
        free(history[oldest_idx].id);
        free_history_exercises(&history[oldest_idx].exercises);

        arrdel(history, oldest_idx);
        *history_ptr = history;
    }
}

void free_draft_routine(Routine* draft) {
    if (draft == NULL) {
        return;
    }

    free(draft->title);
    free(draft->id);

    free_exercises(&draft->exercises);

    free(draft);
}

void free_current_routine(CurrentRoutine* current) {
    if (current == NULL) {
        return;
    }

    if (current->title != NULL) {
        free(current->title);
    }

    if (current->id != NULL) {
        free(current->id);
    }

    free_history_exercises(&current->exercises);

    free(current);
}

void free_routines(Routine** routines_ptr) {
    Routine* routines = *routines_ptr;
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

        free_exercises(&routines[i].exercises);
    }

    arrfree(routines);
}

void free_history(CurrentRoutine** history_ptr) {
    CurrentRoutine* history = *history_ptr;

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

        free_history_exercises(&history[i].exercises);
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
