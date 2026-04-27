#include "utils.h"
#include <bits/time.h>
#include <ctype.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "exercise.h"
#include "json.h"
#include "routine.h"
#include "stb_ds.h"

const int logo_height = 8;
const char* logo[] = {"                       _                  _     ",
                      "                      | |                | |    ",
                      "  __ _ _   _ _ __ ___ | |_ _ __ __ _  ___| | __ ",
                      " / _` | | | | '_ ` _ \\| __| '__/ _` |/ __| |/ / ",
                      "| (_| | |_| | | | | | | |_| | | (_| | (__|   <  ",
                      " \\__, |\\__, |_| |_| |_|\\__|_|  \\__,_|\\___|_|\\_\\ ",
                      "  __/ | __/ |                                   ",
                      " |___/ |___/                                    "};

void init_app_state(AppState* app_state) {
    app_state->routines = NULL;
    app_state->history = NULL;
    app_state->current = NULL;
    app_state->draft.exercises = NULL;
}

void draw_logo(WINDOW* win, int start_y, int start_x) {
    for (int i = 0; i < logo_height; i++) {
        mvwprintw(win, start_y + i, start_x, "%s", logo[i]);
    }
}

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

void serialize_routines(Routine* routines) {
    FILE* file = fopen("workouts.json", "w");
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
void deserialize_routines(Routine** routines) {
    FILE* file = fopen("workouts.json", "ab+");
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
        free(buffer);
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

void serialize_history(CurrentRoutine* routines) {
    FILE* file = fopen("history.json", "w");
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
            CurrentExercise ex = routines[i].exercises[j];
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
}

// stores history.json into routines struct array
void deserialize_history(CurrentRoutine** routines) {
    FILE* file = fopen("history.json", "ab+");
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
        free(buffer);
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

        CurrentRoutine r;
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
                    CurrentExercise ex;

                    struct json_object_element_s* prop = ex_obj->start;
                    while (prop) {
                        if (strcmp(prop->name->string, "title") == 0) {
                            ex.title = strdup(json_value_as_string(prop->value)->string);
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

void start_stopwatch(Stopwatch* sw) {
    sw->is_running = true;
    clock_gettime(CLOCK_MONOTONIC, &sw->start_time);
}

void stop_stopwatch(Stopwatch* sw) {
    if (sw->is_running) {
        clock_gettime(CLOCK_MONOTONIC, &sw->stop_time);
        sw->is_running = false;
    }
}

double get_stopwatch(Stopwatch* sw) {
    struct timespec current_time;

    if (sw->is_running) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
    } else {
        current_time = sw->stop_time;
    }

    double start_sec = sw->start_time.tv_sec + (sw->start_time.tv_nsec / 1e9);
    double current_sec = current_time.tv_sec + (current_time.tv_nsec / 1e9);

    return current_sec - start_sec;
}

void free_app_state(AppState* app_state) {
    free(app_state->stopwatch);
    free(app_state->draft.title);
    free_current_routine(app_state->current);
    free_routines(app_state->routines);
    free_history(app_state->history);
}

void generate_uuid(char* buf) {
    srand(time(NULL));
    const char* hex_chars = "0123456789abcdef";

    for (int i = 0; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            buf[i] = '-';
        } else if (i == 14) {
            buf[i] = '4';
        } else if (i == 19) {
            buf[i] = hex_chars[(rand() % 4) + 8];
        } else {
            buf[i] = hex_chars[rand() % 16];
        }
    }
    buf[36] = '\0';
}
