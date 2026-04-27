#include "exercise.h"
#include "form.h"
#include "stb_ds.h"

void exercise_arr_add(Exercise** exercises, Form form) {
    Exercise ex;
    ex.title = get_field_value(&form.fields[0]);
    char* sets_str = get_field_value(&form.fields[1]);
    char* reps_str = get_field_value(&form.fields[2]);

    ex.sets = atoi(sets_str) <= 0 ? 1 : atoi(sets_str);
    ex.reps = atoi(reps_str) <= 0 ? 1 : atoi(reps_str);

    free(sets_str);
    free(reps_str);

    arrput(*exercises, ex);
}
