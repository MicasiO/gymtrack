#include "serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "json.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

char* get_storage_path() {
    char* home_dir = getenv("HOME");
    if (home_dir == NULL) {
        return strdup(".");
    }

    size_t path_len = strlen(home_dir) + strlen(APP_DIR_NAME) + 2;
    char* full_path = malloc(path_len);

    snprintf(full_path, path_len, "%s/%s", home_dir, APP_DIR_NAME);

    struct stat st = {0};

    if (stat(full_path, &st) == -1) {
        if (mkdir(full_path, 0700) != 0) {
            die("failed to create storage path");
        }
    } else {
        if (!S_ISDIR(st.st_mode)) {
            die("storage path failed");
        }
    }

    return full_path;
}

void serialize_routines(Routine* routines) {
    char* dir = get_storage_path();

    char workout_dir[1024];
    snprintf(workout_dir, sizeof(workout_dir), "%s/workouts.json", dir);

    FILE* file = fopen(workout_dir, "w");
    if (!file) {
        die("file");
    }

    int count = arrlen(routines);

    fprintf(file, "[\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "    {\n");

        fprintf(file, "        \"id\": \"%s\",\n", routines[i].id);
        fprintf(file, "        \"title\": \"%s\",\n", routines[i].title);
        fprintf(file, "        \"last_done\": %ld,\n", routines[i].last_done);
        fprintf(file, "        \"exercises\": [\n");
        int ex_count = arrlen(routines[i].exercises);
        for (int j = 0; j < ex_count; j++) {
            fprintf(file, "            {\n");
            Exercise ex = routines[i].exercises[j];
            fprintf(file, "               \"id\": \"%s\",\n", ex.id);
            fprintf(file, "               \"title\": \"%s\",\n", ex.title);
            fprintf(file, "               \"sets\": %d,\n", ex.sets);
            fprintf(file, "               \"reps\": %d\n", ex.reps);

            if (j == ex_count - 1) {
                fprintf(file, "            }\n");
            } else {
                fprintf(file, "            },\n");
            }
        }
        fprintf(file, "        ]\n");

        if (i == count - 1) {
            fprintf(file, "    }\n");
        } else {
            fprintf(file, "    },\n");
        }
    }

    fprintf(file, "]\n");
    fclose(file);
    free(dir);
}

// stores workout.json into routines struct array
void deserialize_routines(Routine** routines) {
    char* dir = get_storage_path();

    char workout_dir[1024];
    snprintf(workout_dir, sizeof(workout_dir), "%s/workouts.json", dir);

    FILE* file = fopen(workout_dir, "r");
    if (file == NULL) {
        *routines = NULL;
        free(dir);
        return;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(len + 1);
    if (!buffer) {
        die("malloc");
    }

    fread(buffer, 1, len, file);
    buffer[len] = '\0';
    fclose(file);

    if (len == 0) {
        free(buffer);
        *routines = NULL;
        free(dir);
        return;
    }

    struct json_value_s* root = json_parse(buffer, len);
    if (!root) {
        die("invalid JSON");
    }

    struct json_array_s* routines_arr = (struct json_array_s*)json_value_as_array(root);
    if (!routines_arr) {
        die("empty workout");
    }
    struct json_array_element_s* routine_el = routines_arr->start;

    for (; routine_el != NULL; routine_el = routine_el->next) {
        struct json_object_s* routine_obj = json_value_as_object(routine_el->value);

        Routine r;
        r.exercises = NULL;

        struct json_object_element_s* prop = routine_obj->start;
        while (prop) {
            if (strcmp(prop->name->string, "title") == 0) {
                r.title = strdup(json_value_as_string(prop->value)->string);
            } else if (strcmp(prop->name->string, "id") == 0) {
                r.id = strdup(json_value_as_string(prop->value)->string);
            } else if (strcmp(prop->name->string, "last_done") == 0) {
                r.last_done = (time_t)atoll(json_value_as_number(prop->value)->number);
            } else if (strcmp(prop->name->string, "exercises") == 0) {
                struct json_array_s* ex_arr = json_value_as_array(prop->value);
                struct json_array_element_s* ex_el = ex_arr->start;

                while (ex_el) {
                    struct json_object_s* ex_obj = json_value_as_object(ex_el->value);
                    Exercise ex;

                    struct json_object_element_s* prop = ex_obj->start;
                    while (prop) {
                        if (strcmp(prop->name->string, "title") == 0) {
                            ex.title = strdup(json_value_as_string(prop->value)->string);
                        } else if (strcmp(prop->name->string, "id") == 0) {
                            ex.id = strdup(json_value_as_string(prop->value)->string);
                        } else if (strcmp(prop->name->string, "sets") == 0) {
                            ex.sets = atoi(json_value_as_number(prop->value)->number);
                        } else if (strcmp(prop->name->string, "reps") == 0) {
                            ex.reps = atoi(json_value_as_number(prop->value)->number);
                        }
                        prop = prop->next;
                    }

                    arrput(r.exercises, ex);
                    ex_el = ex_el->next;
                }
            }

            prop = prop->next;
        }

        arrput(*routines, r);
    }

    free(root);
    free(buffer);
    free(dir);
}

void serialize_history(CurrentRoutine* routines) {
    char* dir = get_storage_path();

    char history_dir[1024];
    snprintf(history_dir, sizeof(history_dir), "%s/history.json", dir);

    FILE* file = fopen(history_dir, "w");
    if (!file) {
        die("file");
    }

    int count = arrlen(routines);

    fprintf(file, "[\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "    {\n");

        fprintf(file, "        \"id\": \"%s\",\n", routines[i].id);
        fprintf(file, "        \"title\": \"%s\",\n", routines[i].title);
        fprintf(file, "        \"duration\": %d,\n", routines[i].duration);
        fprintf(file, "        \"last_done\": %ld,\n", routines[i].last_done);
        fprintf(file, "        \"exercises\": [\n");
        int ex_count = arrlen(routines[i].exercises);
        for (int j = 0; j < ex_count; j++) {
            fprintf(file, "            {\n");
            CurrentExercise ex = routines[i].exercises[j];
            fprintf(file, "               \"id\": \"%s\",\n", ex.id);
            fprintf(file, "               \"title\": \"%s\",\n", ex.title);
            fprintf(file, "               \"sets\": %d,\n", ex.sets);
            fprintf(file, "               \"reps\": [");
            for (int r = 0; r < ex.sets; r++) {
                if (r == ex.sets - 1) {
                    fprintf(file, "%d],\n", ex.reps[r]);
                } else {
                    fprintf(file, "%d, ", ex.reps[r]);
                }
            }
            fprintf(file, "               \"done\": %d\n", ex.done);
            if (j == ex_count - 1) {
                fprintf(file, "            }\n");
            } else {
                fprintf(file, "            },\n");
            }
        }
        fprintf(file, "        ]\n");

        if (i == count - 1) {
            fprintf(file, "    }\n");
        } else {
            fprintf(file, "    },\n");
        }
    }

    fprintf(file, "]\n");
    fclose(file);
    free(dir);
}

// stores history.json into routines struct array
void deserialize_history(CurrentRoutine** routines) {
    char* dir = get_storage_path();

    char history_dir[1024];
    snprintf(history_dir, sizeof(history_dir), "%s/history.json", dir);

    FILE* file = fopen(history_dir, "r");
    if (file == NULL) {
        *routines = NULL;
        free(dir);
        return;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = malloc(len + 1);
    if (!buffer) {
        die("malloc");
    }
    fread(buffer, 1, len, file);
    buffer[len] = '\0';
    fclose(file);

    if (len == 0) {
        free(buffer);
        *routines = NULL;
        free(dir);
        return;
    }

    struct json_value_s* root = json_parse(buffer, len);
    if (!root) {
        die("invalid JSON");
    }

    struct json_array_s* routines_arr = (struct json_array_s*)json_value_as_array(root);
    if (!routines_arr) {
        die("empty workout");
    }
    struct json_array_element_s* routine_el = routines_arr->start;

    for (; routine_el != NULL; routine_el = routine_el->next) {
        struct json_object_s* routine_obj = json_value_as_object(routine_el->value);

        CurrentRoutine r;
        r.exercises = NULL;

        struct json_object_element_s* prop = routine_obj->start;
        while (prop) {
            if (strcmp(prop->name->string, "title") == 0) {
                r.title = strdup(json_value_as_string(prop->value)->string);
            } else if (strcmp(prop->name->string, "duration") == 0) {
                r.duration = atoi(json_value_as_number(prop->value)->number);
            } else if (strcmp(prop->name->string, "id") == 0) {
                r.id = strdup(json_value_as_string(prop->value)->string);
            } else if (strcmp(prop->name->string, "last_done") == 0) {
                r.last_done = (time_t)atoll(json_value_as_number(prop->value)->number);
            } else if (strcmp(prop->name->string, "exercises") == 0) {
                struct json_array_s* ex_arr = json_value_as_array(prop->value);
                struct json_array_element_s* ex_el = ex_arr->start;

                while (ex_el) {
                    struct json_object_s* ex_obj = json_value_as_object(ex_el->value);
                    CurrentExercise ex;

                    struct json_object_element_s* prop = ex_obj->start;
                    while (prop) {
                        if (strcmp(prop->name->string, "title") == 0) {
                            ex.title = strdup(json_value_as_string(prop->value)->string);
                        } else if (strcmp(prop->name->string, "id") == 0) {
                            ex.id = strdup(json_value_as_string(prop->value)->string);
                        } else if (strcmp(prop->name->string, "sets") == 0) {
                            ex.sets = atoi(json_value_as_number(prop->value)->number);
                        } else if (strcmp(prop->name->string, "reps") == 0) {
                            struct json_array_s* reps_arr = json_value_as_array(prop->value);
                            struct json_array_element_s* reps_el = reps_arr->start;
                            ex.reps = NULL;
                            while (reps_el) {
                                arrput(ex.reps, atoi(json_value_as_number(reps_el->value)->number));
                                reps_el = reps_el->next;
                            }
                        } else if (strcmp(prop->name->string, "done") == 0) {
                            ex.done = atoi(json_value_as_number(prop->value)->number);
                        }

                        prop = prop->next;
                    }

                    arrput(r.exercises, ex);
                    ex_el = ex_el->next;
                }
            }

            prop = prop->next;
        }

        arrput(*routines, r);
    }

    free(root);
    free(buffer);
    free(dir);
}
