#include <ctype.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "exercise.h"
#include "json.h"
#include "routine.h"
#include "stb_ds.h"

void die(const char* s) {
    endwin();
    perror(s);
    exit(1);
}

int is_str_empty(char* str) {
    if (str == NULL)
        return 1;
    while (*str) {
        if (!isspace((unsigned char)*str))
            return 0;
        str++;
    }
    return 1;
}

void serialize(Routine* routines) {
    FILE* file = fopen("workouts.json", "w");
    if (!file) {
        die("file");
    }

    int count = arrlen(routines);

    fprintf(file, "[\n");
    for (int i = 0; i < count; i++) {
        fprintf(file, "    {\n");

        fprintf(file, "        \"id\": %lld,\n", routines[i].id);
        fprintf(file, "        \"title\": \"%s\",\n", routines[i].title);
        fprintf(file, "        \"last_done\": 0,\n");
        fprintf(file, "        \"exercises\": [\n");
        int ex_count = arrlen(routines[i].exercises);
        for (int j = 0; j < ex_count; j++) {
            fprintf(file, "            {\n");
            Exercise ex = routines[i].exercises[j];
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
}

// stores workout.json into routines struct array
void deserialize(Routine** routines) {
    FILE* file = fopen("workouts.json", "r");
    if (file == NULL) {
        die("data file");
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
        *routines = NULL;
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
                r.id = atoll(json_value_as_number(prop->value)->number);
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
}

void print_log(const char* s, ...) {
    FILE* file = fopen("logs.txt", "a+");
    if (!file) {
        die("file");
    }

    va_list args;
    va_start(args, s);

    vfprintf(file, s, args);

    fflush(file);
    fclose(file);

    va_end(args);
}
