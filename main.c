#define STB_DS_IMPLEMENTATION
#include <assert.h>
#include <ncurses.h>
#include <ncurses/form.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "create.h"
#include "json.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

void show_header(WINDOW* header_win) {
    wattron(header_win, A_BOLD);
    wprintw(header_win, "gymtrack");
    wattroff(header_win, A_BOLD);
    refresh();
    wrefresh(header_win);
}

enum state show_menu(WINDOW* body_win, Routine* routines) {
    wclear(body_win);
    WINDOW* win = newwin(13, 48, 2, 0);
    box(win, 0, 0);
    curs_set(0);
    mvwprintw(win, 2, 2, "Workouts:");
    for (int i = 0; i < arrlen(routines); i++) {
        mvwprintw(win, i + 4, 3, "[%d] %s", i + 1, routines[i].title);
    }
    mvwprintw(win, 10, 2, "[q]uit | [s]tart workout | [c]reate workout");

    wrefresh(win);

    enum state next_state = -1;
    while (next_state == -1) {
        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                die("char input");
                break;
            case 'q':
                next_state = STATE_QUIT;
                break;
            case 'c':
                next_state = STATE_CREATE_TITLE;
                break;
            default:
                break;
        }
    }

    delwin(win);
    return next_state;
}

// stores workout.json into routines struct array
void parse_data(Routine** routines) {
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

int main() {
    Routine* routines = NULL;
    parse_data(&routines);

    initscr();
    set_escdelay(25);
    noecho();
    curs_set(0);
    cbreak();
    int sx, sy;
    getmaxyx(stdscr, sy, sx);
    WINDOW* header_win = newwin(2, sx, 0, 0);
    WINDOW* body_win = newwin(arrlen(routines) + 7, sx, 2, 0);

    enum state state;

    // draft routine for creation
    Routine draft;
    draft.exercises = NULL;

    show_header(header_win);
    state = show_menu(body_win, routines);

    while (state != STATE_QUIT) {
        touchwin(stdscr);
        refresh();

        touchwin(header_win);
        wrefresh(header_win);

        touchwin(body_win);
        wrefresh(body_win);
        switch (state) {
            case STATE_MENU:
                state = show_menu(body_win, routines);
                break;
            case STATE_CREATE_TITLE:
                state = show_create_title(body_win, &routines, &draft);
                break;
            case STATE_CREATE_WORKOUT:
                state = show_create_workout(body_win, &routines, &draft);
                break;
            case STATE_CREATE_CONTINUE:
                state = show_create_continue(body_win, &routines, &draft);
                break;
            default:
                break;
        }
    }

    delwin(header_win);
    delwin(body_win);
    endwin();
    return 0;
}
