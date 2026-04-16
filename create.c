#include "create.h"
#include <ncurses.h>
#include <ncurses/form.h>
#include <stdlib.h>
#include "exercise.h"
#include "form.h"
#include "stb_ds.h"
#include "utils.h"

enum state show_create_title(WINDOW* body_win, Routine** routines, Routine* draft) {
    draft->title = NULL;
    draft->exercises = NULL;
    wclear(body_win);
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
    handle_input_form(&form, &next_state, STATE_MENU, STATE_CREATE_WORKOUT);

    if (next_state == STATE_CREATE_WORKOUT) {
        draft->title = get_field_value(&form.fields[0]);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(body_win);

    return next_state;
}

enum state show_create_workout(WINDOW* body_win, Routine** routines, Routine* draft) {
    wclear(body_win);
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
        Exercise ex;
        ex.title = get_field_value(&form.fields[0]);
        ex.sets = atoi(get_field_value(&form.fields[1]));
        ex.reps = atoi(get_field_value(&form.fields[2]));
        arrput(draft->exercises, ex);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(body_win);

    return next_state;
}

enum state show_create_continue(WINDOW* body_win, Routine** routines, Routine* draft) {
    wclear(body_win);
    WINDOW* win = newwin(20, 40, 2, 0);
    box(win, 0, 0);
    curs_set(0);

    WINDOW* form_win = derwin(win, 10, 38, 1, 1);

    Form form;
    init_form(win, form_win, &form, 1, 1);

    show_form(&form);
    mvwprintw(win, 2, 2, "Workout title: %s", draft->title);

    for (int i = 0; i < arrlen(draft->exercises); i++) {
        if (i == 10) {
            mvwprintw(win, 14, 2, "...");
            break;
        }
        Exercise ex = draft->exercises[arrlen(draft->exercises) - i - 1];
        mvwprintw(win, 4 + i, 2, "%s: %d x %d", ex.title, ex.sets, ex.reps);
    }
    mvwprintw(win, 17, 2, "[esc] finish | [enter] add exercise");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_MENU, STATE_CREATE_WORKOUT);

    if (next_state == STATE_MENU) {
        arrput(*routines, *draft);
        serialize(*routines);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(body_win);

    return next_state;
}
