#include "menu.h"
#include <ncurses.h>
#include "exercise.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

void select_workout(AppState* app_state, enum state* next_state, const char ch, int curr_idx) {
    int idx = curr_idx + (ch - '0') - 1;
    if (idx > arrlen(app_state->routines)) {
        return;
    }
    app_state->current = &app_state->routines[idx];
    *next_state = STATE_MENU_WORKOUT;
}

enum state show_main_menu(AppState* app_state) {
    wclear(app_state->body_win);
    WINDOW* win = newwin(15, 48, 2, 0);
    curs_set(0);
    keypad(win, true);

    int curr_idx = 0;

    enum state next_state = -1;
    while (next_state == -1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 2, 2, "Workouts:");

        for (int i = 0; i < 5; i++) {
            if (i + curr_idx >= arrlen(app_state->routines)) {
                break;
            }
            mvwprintw(win, i + 4, 3, "[%d] %s", i + 1, app_state->routines[i + curr_idx].title);
        }

        if (arrlen(app_state->routines) != 0) {
            mvwprintw(win, 10, 2, "[1..5] select workout");
        } else {
            mvwprintw(win, 5, 2, "No workouts");
        }

        if (arrlen(app_state->routines) > 5) {
            if (curr_idx > 0 && curr_idx + 5 < arrlen(app_state->routines)) {
                mvwprintw(win, 11, 2, "[p]revious | [n]ext");
            } else if (curr_idx > 0) {
                mvwprintw(win, 11, 2, "[p]revious");
            } else if (curr_idx + 5 < arrlen(app_state->routines)) {
                mvwprintw(win, 11, 2, "[n]ext");
            }
        }

        mvwprintw(win, 12, 2, "[q]uit | [c]reate workout");

        wrefresh(win);

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
            case 'p':
                if (curr_idx != 0) {
                    curr_idx -= 5;
                }
                break;
            case 'n':
                if (curr_idx + 5 < arrlen(app_state->routines)) {
                    curr_idx += 5;
                }
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
                if (arrlen(app_state->routines) != 0) {
                    select_workout(app_state, &next_state, ch, curr_idx);
                }
                break;
            default:
                break;
        }
    }

    delwin(win);
    return next_state;
}

enum state show_workout_menu(AppState* app_state) {
    wclear(app_state->body_win);
    WINDOW* win = newwin(21, 40, 2, 0);
    curs_set(0);
    keypad(win, true);

    Routine current_routine = *app_state->current;

    int curr_idx = 0;
    enum state next_state = -1;

    while (next_state == -1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 2, 2, "%s:", current_routine.title);

        for (int i = 0; i < 10; i++) {
            Exercise ex = current_routine.exercises[i + curr_idx];
            if (i + curr_idx >= arrlen(app_state->current->exercises)) {
                break;
            }
            mvwprintw(win, i + 4, 3, "%s: %d x %d", ex.title, ex.reps, ex.sets);
        }

        if (arrlen(current_routine.exercises) > 5) {
            if (curr_idx > 0 && curr_idx + 5 < arrlen(current_routine.exercises)) {
                mvwprintw(win, 13, 2, "[p]revious | [n]ext");
            } else if (curr_idx > 0) {
                mvwprintw(win, 13, 2, "[p]revious");
            } else if (curr_idx + 5 < arrlen(current_routine.exercises)) {
                mvwprintw(win, 13, 2, "[n]ext");
            }
        }
        mvwprintw(win, 17, 2, "[esc] back | [enter] start");
        mvwprintw(win, 18, 2, "[d]elete");

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                die("char input");
                break;
            case 27:  // esc
                next_state = STATE_MENU_MAIN;
                break;
            case 10:  // enter
                // next_state = STATE_RUN;
                break;
            case 'd':
                routine_arr_remove(&app_state->routines, &current_routine);
                next_state = STATE_MENU_MAIN;
                break;
            case 'p':
                if (curr_idx != 0) {
                    curr_idx -= 5;
                }
                break;
            case 'n':
                if (curr_idx + 5 < arrlen(app_state->routines)) {
                    curr_idx += 5;
                }
                break;
            default:
                break;
        }
    }

    delwin(win);
    return next_state;
}
