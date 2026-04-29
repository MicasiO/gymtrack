#include "utils.h"
#include <bits/time.h>
#include <ctype.h>
#include <ncurses.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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
    if (app_state->stopwatch != NULL) {
        free(app_state->stopwatch);
    }
    if (app_state->draft.title != NULL) {
        free(app_state->draft.title);
    }
    if (app_state->current != NULL) {
        free_current_routine(app_state->current);
    }
    if (app_state->routines != NULL) {
        free_routines(&app_state->routines);
    }
    if (app_state->history != NULL) {
        free_history(&app_state->history);
    }
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

void mvwprintw_vertical(WINDOW* win, int start_y, int start_x, const char* text) {
    if (text == NULL || win == NULL)
        return;

    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        if (start_y + i >= max_y) {
            break;
        }

        mvwaddch(win, start_y + i, start_x, text[i]);
    }
}
