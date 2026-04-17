#include "routine.h"
#include "form.h"
#include "stb_ds.h"
#include "utils.h"

void routine_arr_add(Routine** routines_ptr, Routine routine) {
    Routine* routines = *routines_ptr;
    routine.id = generate_routine_id(routines);
    arrput(routines, routine);
    serialize(routines);
}

void routine_arr_remove(Routine** routines_ptr, Routine* routine) {
    Routine* routines = *routines_ptr;
    for (int i = 0; i < arrlen(routines); i++) {
        if (routines[i].id == routine->id) {
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
    serialize(*routines_ptr);
}

void exercise_arr_add(Exercise** exercises, Form form) {
    Exercise ex;
    ex.title = get_field_value(&form.fields[0]);
    ex.sets = atoi(get_field_value(&form.fields[1]));
    ex.reps = atoi(get_field_value(&form.fields[2]));
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
