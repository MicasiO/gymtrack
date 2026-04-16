#include <ctype.h>
#include <ncurses.h>
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
