#include <locale.h>
#include "active.h"
#define STB_DS_IMPLEMENTATION
#include <assert.h>
#include <ncurses.h>
#include <ncurses/form.h>
#include "create.h"
#include "menu.h"
#include "stb_ds.h"
#include "utils.h"

void show_header(WINDOW* header_win) {
    draw_logo(header_win, 0, 0);
    wrefresh(header_win);
}

int main() {
    AppState app_state;

    init_app_state(&app_state);

    deserialize_routines(&app_state.routines);
    deserialize_history(&app_state.history);

    setlocale(LC_ALL, "");
    initscr();
    set_escdelay(25);
    noecho();
    curs_set(0);
    cbreak();

    int sx, sy;
    getmaxyx(stdscr, sy, sx);

    WINDOW* header_win = newwin(9, sx, 0, 0);
    WINDOW* body_win = newwin(sy, sx, 9, 0);
    if (header_win == NULL || body_win == NULL) {
        die("window");
    }

    app_state.body_win = body_win;

    enum state state;

    show_header(header_win);
    state = show_main_menu(&app_state);

    while (state != STATE_QUIT) {
        touchwin(stdscr);
        refresh();

        touchwin(header_win);
        wrefresh(header_win);

        touchwin(body_win);
        werase(body_win);
        wrefresh(body_win);
        switch (state) {
            case STATE_MENU_MAIN:
                state = show_main_menu(&app_state);
                break;
            case STATE_MENU_WORKOUT:
                state = show_workout_menu(&app_state);
                break;
            case STATE_CREATE_TITLE:
                state = show_create_title(&app_state);
                break;
            case STATE_CREATE_WORKOUT:
                state = show_create_workout(&app_state);
                break;
            case STATE_CREATE_CANCEL_EX:  // cancel exercise creation
            case STATE_CREATE_CONTINUE:
                state = show_create_continue(&app_state);
                break;
            case STATE_ACTIVE:  // active workout, show all exercises
                state = show_active(&app_state);
                break;
            case STATE_ACTIVE_SETS:  // exercise finished, enter sets
                state = show_active_sets(&app_state);
                break;
            case STATE_ACTIVE_REPS:  // exercise finished, enter reps for each set
                state = show_active_reps(&app_state);
                break;
            case STATE_ACTIVE_FINISHED:  // session finished, save routine to history
                state = show_finish_routine(&app_state);
                break;
            default:
                break;
        }
    }

    free_app_state(&app_state);
    delwin(header_win);
    delwin(body_win);
    endwin();
    return 0;
}
