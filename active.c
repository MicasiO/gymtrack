#include "active.h"
#include <ncurses.h>
#include "exercise.h"
#include "form.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

enum state show_active(AppState* app_state) {
    wclear(app_state->body_win);

    WINDOW* win = derwin(app_state->body_win, WIN_HEIGHT, WIN_WIDTH, 0, 0);
    if (win == NULL) {
        die("window");
    }

    curs_set(0);
    keypad(win, true);

    CurrentRoutine* previous = get_last_routine(app_state->history, app_state->current->routine_id);
    CurrentExercise* prev_exercises = NULL;
    if (previous != NULL) {
        prev_exercises = previous->exercises;
    }

    CurrentExercise* exercises = app_state->current->exercises;
    char* title = app_state->current->title;

    int curr_idx = 0;

    enum state next_state = -1;
    while (next_state == -1) {
        wclear(win);
        box(win, 0, 0);
        mvwprintw(win, 2, 2, "%s", title);

        wattron(win, A_REVERSE);
        mvwprintw(win, 5, 2, "%s%s", exercises[curr_idx].done ? "✔ " : "",
                  exercises[curr_idx].title);
        wattroff(win, A_REVERSE);
        wprintw(win, " %d x %d", exercises[curr_idx].sets, exercises[curr_idx].reps);

        if (arrlen(exercises) > 1) {
            if (curr_idx > 0 && curr_idx < arrlen(exercises) - 1) {
                wattron(win, A_DIM);
                mvwprintw(win, 4, 2, "%s%s", exercises[curr_idx - 1].done ? "✔ " : "",
                          exercises[curr_idx - 1].title);
                wattroff(win, A_DIM);

                wattron(win, A_DIM);
                mvwprintw(win, 6, 2, "%s%s", exercises[curr_idx + 1].done ? "✔ " : "",
                          exercises[curr_idx + 1].title);
                wattroff(win, A_DIM);

                mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious | [n]ext");
            } else if (curr_idx > 0) {
                wattron(win, A_DIM);
                mvwprintw(win, 4, 2, "%s%s", exercises[curr_idx - 1].done ? "✔ " : "",
                          exercises[curr_idx - 1].title);

                wattroff(win, A_DIM);

                mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious");
            } else if (curr_idx < arrlen(exercises) - 1) {
                wattron(win, A_DIM);
                mvwprintw(win, 6, 2, "%s%s", exercises[curr_idx + 1].done ? "✔ " : "",
                          exercises[curr_idx + 1].title);

                wattroff(win, A_DIM);

                mvwprintw(win, WIN_HEIGHT - 4, 2, "[n]ext");
            }
        }

        if (previous != NULL) {
            mvwprintw(win, WIN_HEIGHT - 5, 2, "Last time: %d x %d", prev_exercises[curr_idx].sets,
                      prev_exercises[curr_idx].reps);
        }

        mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] finished exercise");

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                die("char input");
                break;
            case 10:  // enter
                app_state->current->index = curr_idx;
                next_state = STATE_ACTIVE_CONTINUE;
                break;
            case 27:  // esc
                next_state = STATE_MENU_MAIN;
                break;
            case 'p':
                if (curr_idx != 0) {
                    curr_idx--;
                }
                break;
            case 'n':
                if (curr_idx < arrlen(exercises) - 1) {
                    curr_idx++;
                }
                break;
            default:
                break;
        }
    }

    delwin(win);
    return next_state;
}

// shown when exercise is finished to enter reps and sets
enum state show_finish_exercise(AppState* app_state) {
    wclear(app_state->body_win);

    WINDOW* win = derwin(app_state->body_win, WIN_HEIGHT, WIN_WIDTH, 0, 0);
    if (win == NULL) {
        die("window");
    }

    box(win, 0, 0);
    curs_set(1);

    WINDOW* form_win = derwin(win, 9, 42, 1, 1);
    if (form_win == NULL) {
        die("window");
    }

    keypad(form_win, TRUE);

    Form form;
    init_form(win, form_win, &form, 4, 1);
    add_field_form(&form, "Sets:", FIELD_NUMBER, 3);
    add_field_form(&form, "Reps:", FIELD_NUMBER, 3);
    show_form(&form);

    int idx = app_state->current->index;

    mvwprintw(win, 2, 2, "%s", app_state->current->exercises[idx].title);
    mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] continue");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_ACTIVE, STATE_ACTIVE_DONE);

    if (next_state == STATE_ACTIVE_DONE) {
        app_state->current->exercises[idx].sets = atoi(get_field_value(&form.fields[0]));
        app_state->current->exercises[idx].reps = atoi(get_field_value(&form.fields[1]));
        app_state->current->exercises[idx].done = true;

        next_state = STATE_ACTIVE;
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}
