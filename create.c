#include "create.h"
#include <ncurses.h>
#include <ncurses/form.h>
#include "exercise.h"
#include "form.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

enum state show_create_title(AppState* app_state) {
    app_state->draft.title = NULL;
    app_state->draft.exercises = NULL;
    wclear(app_state->body_win);
    WINDOW* win = newwin(8, 44, 2, 0);
    box(win, 0, 0);
    curs_set(1);

    WINDOW* form_win = derwin(win, 3, 42, 1, 1);
    keypad(form_win, TRUE);

    Form form;
    init_form(win, form_win, &form, 1, 1);
    add_field_form(&form, "Workout title:", FIELD_STRING, 20);

    show_form(&form);
    mvwprintw(win, 5, 2, "[esc] return to menu | [enter] continue");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_MENU_MAIN, STATE_CREATE_WORKOUT);

    if (next_state == STATE_CREATE_WORKOUT) {
        app_state->draft.title = get_field_value(&form.fields[0]);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}

enum state show_create_workout(AppState* app_state) {
    wclear(app_state->body_win);
    WINDOW* win = newwin(14, 44, 2, 0);
    box(win, 0, 0);
    curs_set(1);

    WINDOW* form_win = derwin(win, 9, 42, 1, 1);
    keypad(form_win, TRUE);

    Form form;
    init_form(win, form_win, &form, 1, 1);
    add_field_form(&form, "Exercise title:", FIELD_STRING, 20);
    add_field_form(&form, "Sets:", FIELD_NUMBER, 3);
    add_field_form(&form, "Reps:", FIELD_NUMBER, 3);

    show_form(&form);
    mvwprintw(win, 11, 2, "[esc] back | [enter] continue");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_CREATE_TITLE, STATE_CREATE_CONTINUE);

    if (next_state == STATE_CREATE_CONTINUE) {
        exercise_arr_add(&app_state->draft.exercises, form);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}

enum state show_create_continue(AppState* app_state) {
    wclear(app_state->body_win);
    WINDOW* win = newwin(20, 40, 2, 0);
    box(win, 0, 0);
    curs_set(0);

    WINDOW* form_win = derwin(win, 10, 38, 1, 1);

    Form form;
    init_form(win, form_win, &form, 1, 1);

    show_form(&form);
    mvwprintw(win, 2, 2, "Workout title: %s", app_state->draft.title);

    for (int i = 0; i < arrlen(app_state->draft.exercises); i++) {
        if (i == 10) {
            mvwprintw(win, 14, 2, "...");
            break;
        }
        Exercise ex = app_state->draft.exercises[arrlen(app_state->draft.exercises) - i - 1];
        mvwprintw(win, 4 + i, 2, "%s: %d x %d", ex.title, ex.sets, ex.reps);
    }
    mvwprintw(win, 17, 2, "[esc] finish | [enter] add exercise");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_MENU_MAIN, STATE_CREATE_WORKOUT);

    if (next_state == STATE_MENU_MAIN) {
        routine_arr_add(&app_state->routines, app_state->draft);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}
