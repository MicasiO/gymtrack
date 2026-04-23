#include "active.h"
#include <ncurses.h>
#include <stdlib.h>
#include "exercise.h"
#include "form.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

enum state show_active(AppState* app_state) {
    werase(app_state->body_win);

    WINDOW* win = derwin(app_state->body_win, WIN_HEIGHT, WIN_WIDTH, 0, 0);
    if (win == NULL) {
        die("window");
    }

    curs_set(0);
    keypad(win, true);

    wtimeout(win, 100);  // prevent input blocking for timer

    CurrentRoutine* previous = get_last_routine(app_state->history, app_state->current->routine_id);
    CurrentExercise* prev_exercises = NULL;
    if (previous != NULL) {
        prev_exercises = previous->exercises;
    }

    CurrentExercise* exercises = app_state->current->exercises;
    char* title = app_state->current->title;

    int curr_idx = app_state->current->index;

    enum state next_state = -1;
    while (next_state == -1) {
        werase(win);
        box(win, 0, 0);

        double total_seconds = get_stopwatch(app_state->stopwatch);
        int hours = (int)total_seconds / 3600;
        int minutes = ((int)total_seconds % 3600) / 60;
        int seconds = (int)total_seconds % 60;

        mvwprintw(win, 3, 2, "%02d:%02d:%02d", hours, minutes, seconds);

        mvwprintw(win, 2, 2, "%s", title);

        int center_y = 8;

        for (int i = -4; i <= 4; i++) {
            int draw_idx = curr_idx + i;

            if (draw_idx >= 0 && draw_idx < arrlen(exercises)) {
                int draw_y = center_y + i;

                if (i == 0) {
                    wattron(win, A_REVERSE);
                    mvwprintw(win, draw_y, 2, "%s%s", exercises[draw_idx].done ? "✔ " : "",
                              exercises[draw_idx].title);
                    wattroff(win, A_REVERSE);

                    wprintw(win, " %d x ", exercises[draw_idx].sets);
                    draw_formatted_reps(win, exercises[draw_idx].sets, exercises[draw_idx].reps);

                } else {
                    wattron(win, A_DIM);
                    mvwprintw(win, draw_y, 2, "%s%s", exercises[draw_idx].done ? "✔ " : "",
                              exercises[draw_idx].title);
                    wattroff(win, A_DIM);
                }
            }
        }

        if (curr_idx > 0 && curr_idx < arrlen(exercises) - 1) {
            mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious | [n]ext");
        } else if (curr_idx > 0) {
            mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious");
        } else if (curr_idx < arrlen(exercises) - 1) {
            mvwprintw(win, WIN_HEIGHT - 4, 2, "[n]ext");
        }

        if (previous != NULL) {
            mvwprintw(win, WIN_HEIGHT - 7, 2, "Last time: %d x ", prev_exercises[curr_idx].sets);
            draw_formatted_reps(win, prev_exercises[curr_idx].sets, prev_exercises[curr_idx].reps);
        }

        mvwprintw(win, WIN_HEIGHT - 5, 2, "[f] finish session");
        mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] finished exercise");

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                continue;
            case 10:  // enter
                app_state->current->index = curr_idx;
                next_state = STATE_ACTIVE_SETS;
                break;
            case 27:  // esc
                stop_stopwatch(app_state->stopwatch);
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
            case 'f':
                stop_stopwatch(app_state->stopwatch);
                next_state = STATE_ACTIVE_FINISHED;
                break;
            default:
                break;
        }
    }
    wtimeout(win, -1);
    delwin(win);
    return next_state;
}

enum state show_active_sets(AppState* app_state) {
    werase(app_state->body_win);

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
    show_form(&form);

    int idx = app_state->current->index;

    mvwprintw(win, 2, 2, "%s", app_state->current->exercises[idx].title);
    mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] continue");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_ACTIVE, STATE_ACTIVE_REPS);

    if (next_state == STATE_ACTIVE_REPS) {
        CurrentExercise* curr_ex = &app_state->current->exercises[idx];
        app_state->reps_idx = 0;
        char* sets_str = get_field_value(&form.fields[0]);
        curr_ex->sets = atoi(sets_str);
        if (curr_ex->reps != NULL) {
            arrfree(curr_ex->reps);
            curr_ex->reps = NULL;
        }
        app_state->current->exercises[idx].done = true;

        free(sets_str);
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}

enum state show_active_reps(AppState* app_state) {
    werase(app_state->body_win);

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
    add_field_form(&form, "Reps:", FIELD_NUMBER, 3);
    show_form(&form);

    int idx = app_state->current->index;

    mvwprintw(win, 2, 2, "Set %d", app_state->reps_idx + 1);
    mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] continue");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_ACTIVE_SETS, STATE_ACTIVE_REPS);

    if (next_state == STATE_ACTIVE_REPS) {
        CurrentExercise* curr_ex = &app_state->current->exercises[idx];

        char* reps_str = get_field_value(&form.fields[0]);
        arrput(curr_ex->reps, atoi(reps_str));
        // curr_ex.reps[app_state->reps_idx] = atoi(reps_str);
        free(reps_str);

        if (app_state->reps_idx + 1 == curr_ex->sets) {
            next_state = STATE_ACTIVE;
        } else {
            next_state = STATE_ACTIVE_REPS;
            app_state->reps_idx++;
        }
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}

// shown when exercise is finished to enter reps and sets
enum state show_finish_exercise(AppState* app_state) {
    werase(app_state->body_win);

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
    show_form(&form);
    char* sets_str = get_field_value(&form.fields[0]);

    // add_field_form(&form, "Reps:", FIELD_NUMBER, 3);
    free(sets_str);
    delete_form(&form);

    int idx = app_state->current->index;

    mvwprintw(win, 2, 2, "%s", app_state->current->exercises[idx].title);
    mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] continue");
    wrefresh(win);
    wrefresh(form_win);

    enum state next_state;
    handle_input_form(&form, &next_state, STATE_ACTIVE, STATE_ACTIVE_DONE);

    if (next_state == STATE_ACTIVE_DONE) {
        char* sets_str = get_field_value(&form.fields[0]);

        app_state->current->exercises[idx].sets = atoi(sets_str);
        app_state->current->exercises[idx].done = true;

        free(sets_str);

        next_state = STATE_ACTIVE;
    }

    delete_form(&form);
    delwin(form_win);
    delwin(win);

    wrefresh(app_state->body_win);

    return next_state;
}

enum state show_finish_routine(AppState* app_state) {
    werase(app_state->body_win);

    WINDOW* win = derwin(app_state->body_win, WIN_HEIGHT, WIN_WIDTH, 0, 0);
    if (win == NULL) {
        die("window");
    }

    curs_set(0);
    keypad(win, true);

    CurrentRoutine* current_routine = app_state->current;

    int curr_idx = 0;
    enum state next_state = -1;

    while (next_state == -1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 2, 2, "%s:", current_routine->title);

        for (int i = 0; i < 10; i++) {
            if (i + curr_idx >= arrlen(current_routine->exercises)) {
                break;
            }

            CurrentExercise ex = current_routine->exercises[i + curr_idx];
            mvwprintw(win, i + 4, 3, "%s%s: %d x ", ex.done ? "✔ " : "", ex.title, ex.sets);
            draw_formatted_reps(win, ex.sets, ex.reps);
        }

        if (arrlen(current_routine->exercises) > 10) {
            if (curr_idx > 0 && curr_idx + 10 < arrlen(current_routine->exercises)) {
                mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious | [n]ext");
            } else if (curr_idx > 0) {
                mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious");
            } else if (curr_idx + 5 < arrlen(current_routine->exercises)) {
                mvwprintw(win, WIN_HEIGHT - 4, 2, "[n]ext");
            }
        }

        mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back | [enter] continue");

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                die("char input");
                break;
            case 27:  // esc
                next_state = STATE_ACTIVE;
                break;
            case 10:  // enter
                next_state = STATE_MENU_MAIN;
                break;
            case 'p':
                if (curr_idx != 0) {
                    curr_idx -= 10;
                }
                break;
            case 'n':
                if (curr_idx + 10 < arrlen(current_routine->exercises)) {
                    curr_idx += 10;
                }
                break;
            default:
                break;
        }
    }

    delwin(win);
    return next_state;
}

void draw_formatted_reps(WINDOW* win, int sets, int* reps) {
    if (sets < 4) {
        for (int r = 0; r < sets; r++) {
            if (r < sets - 1) {
                wprintw(win, "%d, ", reps[r]);
            } else {
                wprintw(win, "%d", reps[r]);
            }
        }
    } else {
        wprintw(win, "%d,...,%d ", reps[0], reps[sets - 1]);
    }
}
