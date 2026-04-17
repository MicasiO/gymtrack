#define STB_DS_IMPLEMENTATION
#include <assert.h>
#include <ncurses.h>
#include <ncurses/form.h>
#include "create.h"
#include "menu.h"
#include "stb_ds.h"
#include "utils.h"

void show_header(WINDOW* header_win) {
    wattron(header_win, A_BOLD);
    wprintw(header_win, "gymtrack");
    wattroff(header_win, A_BOLD);
    refresh();
    wrefresh(header_win);
}

int main() {
    AppState app_state;
    app_state.routines = NULL;
    app_state.draft.exercises = NULL;

    deserialize(&app_state.routines);

    initscr();
    set_escdelay(25);
    noecho();
    curs_set(0);
    cbreak();
    int sx, sy;
    getmaxyx(stdscr, sy, sx);
    WINDOW* header_win = newwin(2, sx, 0, 0);
    WINDOW* body_win = newwin(20, sx, 2, 0);
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
            case STATE_CREATE_CONTINUE:
                state = show_create_continue(&app_state);
                break;
            default:
                break;
        }
    }

    if (app_state.routines != NULL) {
        arrfree(app_state.routines->exercises);
        arrfree(app_state.routines);
    }
    delwin(header_win);
    delwin(body_win);
    endwin();
    return 0;
}
