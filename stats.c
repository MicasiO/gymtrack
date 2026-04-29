#include "stats.h"
#include <math.h>
#include <ncurses.h>
#include <string.h>
#include "exercise.h"
#include "routine.h"
#include "stb_ds.h"
#include "utils.h"

enum state show_workout_stats(AppState* app_state) {
    WINDOW* win = derwin(app_state->body_win, WIN_HEIGHT, WIN_WIDTH, 0, 0);
    if (win == NULL) {
        die("window");
    }

    curs_set(0);
    keypad(win, true);

    bool empty_exercise_data = false;

    CurrentRoutine** routine_hist =
        get_routine_history(&app_state->history, app_state->current->id);

    if (arrlen(routine_hist) <= 0 || routine_hist == NULL) {
        empty_exercise_data = true;
    }

    enum state next_state = -1;
    while (next_state == -1) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 2, 2, "Stats");

        display_title(win, app_state->current->title);
        display_exercise_count(win, arrlen(app_state->current->exercises));

        if (!empty_exercise_data) {
            display_average_duration(win, routine_hist);
            display_average_completion(win, routine_hist);
        }

        mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back");
        if (!empty_exercise_data) {
            mvwprintw(win, WIN_HEIGHT - 3, WIN_WIDTH - 25, "[e]xercise progression");
        }

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                die("char input");
                break;
            case 27:  // esc
                arrfree(routine_hist);
                next_state = STATE_MENU_WORKOUT;
                break;
            case 'e':
                if (!empty_exercise_data) {
                    arrfree(routine_hist);
                    next_state = STATE_STATS_EXERCISE;
                }
                break;
            default:
                break;
        }
    }

    delwin(win);
    return next_state;
}

enum state show_exercise_stats(AppState* app_state) {
    WINDOW* win = derwin(app_state->body_win, WIN_HEIGHT, WIN_WIDTH, 0, 0);
    if (win == NULL) {
        die("window");
    }

    int chart_h = 13;
    int chart_w = WIN_WIDTH - 8;
    WINDOW* chart_win = derwin(win, chart_h, chart_w, WIN_HEIGHT - chart_h - 5, 6);
    if (chart_win == NULL) {
        die("window");
    }

    curs_set(0);
    keypad(win, true);

    int curr_idx = 0;

    CurrentRoutine** routine_hist_ptr =
        get_routine_history(&app_state->history, app_state->current->id);

    CurrentRoutine* routine_hist = *routine_hist_ptr;
    CurrentExercise** exercise_hist_ptr = NULL;

    enum state next_state = -1;
    while (next_state == -1) {
        if (exercise_hist_ptr != NULL) {
            arrfree(exercise_hist_ptr);
        }
        CurrentExercise current_exercise = routine_hist->exercises[curr_idx];
        exercise_hist_ptr = get_exercise_history(&app_state->history, current_exercise.id);

        werase(win);
        box(win, 0, 0);
        wborder(chart_win, ACS_VLINE, ' ', ' ', ACS_HLINE, ' ', ' ', ACS_LLCORNER, ' ');
        mvwprintw(win, 2, 2, "%s", current_exercise.title);

        display_exercise_chart(win, chart_win, chart_h, chart_w, exercise_hist_ptr);

        if (curr_idx > 0 && curr_idx < arrlen(app_state->current->exercises) - 1) {
            mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious");
            mvwprintw(win, WIN_HEIGHT - 4, WIN_WIDTH - 9, "[n]ext");
        } else if (curr_idx > 0) {
            mvwprintw(win, WIN_HEIGHT - 4, 2, "[p]revious");
        } else if (curr_idx < arrlen(app_state->current->exercises) - 1) {
            mvwprintw(win, WIN_HEIGHT - 4, WIN_WIDTH - 9, "[n]ext");
        }

        mvwprintw(win, WIN_HEIGHT - 3, 2, "[esc] back");

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case ERR:
                die("char input");
                break;
            case 27:  // esc
                if (exercise_hist_ptr != NULL) {
                    arrfree(exercise_hist_ptr);
                }
                arrfree(routine_hist_ptr);
                next_state = STATE_STATS_WORKOUT;
                break;
            case 'p':
                if (curr_idx != 0) {
                    curr_idx -= 1;
                }
                break;
            case 'n':
                if (curr_idx + 1 < arrlen(app_state->current->exercises)) {
                    curr_idx += 1;
                }
                break;
            default:
                break;
        }
    }

    delwin(chart_win);
    delwin(win);
    return next_state;
}

void display_title(WINDOW* win, char* title) {
    mvwprintw(win, 4, 2, "Title:");
    mvwprintw(win, 4, WIN_WIDTH - strlen(title) - 3, "%s", title);
}

void display_exercise_count(WINDOW* win, long count) {
    int digits = (count == 0) ? 1 : log10((double)count) + 1;
    mvwprintw(win, 5, 2, "Exercise count:");
    mvwprintw(win, 5, WIN_WIDTH - 3 - digits, "%ld", count);
}

void display_average_duration(WINDOW* win, CurrentRoutine** history_ptr) {
    int sum = 0;

    for (int i = 0; i < arrlen(history_ptr); i++) {
        sum += history_ptr[i]->duration;
    }

    int avg = sum / arrlen(history_ptr);

    int hours = avg / 3600;
    int minutes = (avg % 3600) / 60;
    int seconds = avg % 60;

    mvwprintw(win, 6, 2, "Average duration:");
    mvwprintw(win, 6, WIN_WIDTH - 11, "%02d:%02d:%02d", hours, minutes, seconds);
}

void display_average_completion(WINDOW* win, CurrentRoutine** history_ptr) {
    int completed_exercises = 0;
    int total_exercises = 0;

    for (int i = 0; i < arrlen(history_ptr); i++) {
        total_exercises += arrlen(history_ptr[i]->exercises);
        for (int j = 0; j < arrlen(history_ptr[i]->exercises); j++) {
            if (history_ptr[i]->exercises[j].done) {
                completed_exercises++;
            }
        }
    }

    int average = ((double)completed_exercises / (double)total_exercises) * 100;

    mvwprintw(win, 7, 2, "Average completion:");
    mvwprintw(win, 7, WIN_WIDTH - ((average == 100) ? 7 : 6), "%d%%", average);
}

void display_exercise_chart(WINDOW* win,
                            WINDOW* chart_win,
                            int chart_h,
                            int chart_w,
                            CurrentExercise** history_ptr) {
    int num_entries = arrlen(history_ptr);
    if (num_entries == 0) {
        mvwprintw(chart_win, 2, 2, "No data to display");
        return;
    }

    int chart_max_y, chart_max_x;
    getmaxyx(chart_win, chart_max_y, chart_max_x);

    int chart_bottom = chart_max_y - 2;
    int chart_max_height = chart_max_y - 4;
    int start_x = 6;
    int available_width = chart_max_x - start_x - 3;
    int spacing = 1;

    int max_allowed_bars = available_width / 2;
    int chunk_size = 1;

    if (num_entries > max_allowed_bars) {
        chunk_size = (num_entries + max_allowed_bars - 1) / max_allowed_bars;
    }

    int* grouped_totals = NULL;
    int max_reps = 1;

    for (int i = num_entries - 1; i >= 0; i -= chunk_size) {
        int chunk_peak = 0;

        for (int j = 0; j < chunk_size; j++) {
            int idx = i - j;
            if (idx < 0)
                break;

            int session_total = 0;
            for (int r = 0; r < arrlen(history_ptr[idx]->reps); r++) {
                session_total += history_ptr[idx]->reps[r];
            }

            if (session_total > chunk_peak) {
                chunk_peak = session_total;
            }
        }

        arrput(grouped_totals, chunk_peak);

        if (chunk_peak > max_reps) {
            max_reps = chunk_peak;
        }
    }

    int drawn_bars = arrlen(grouped_totals);

    int bar_width = (available_width / drawn_bars) - spacing;

    mvwprintw_vertical(win, chart_bottom - chart_max_height + 6, 3, "Reps");
    mvwprintw(win, (WIN_HEIGHT - chart_h - 5) + chart_max_y, chart_max_x / 2 - 4,
              "Sessions (max 50)");

    mvwprintw(win, chart_bottom - chart_max_height + 3, 2, "Max");
    mvwprintw(chart_win, chart_bottom - chart_max_height, 1, " %d", max_reps);

    int current_x = start_x;

    for (int i = 0; i < drawn_bars; i++) {
        if (current_x + bar_width > chart_max_x - 1)
            break;

        double scale = (double)grouped_totals[i] / (double)max_reps;
        int bar_height = (int)(scale * chart_max_height);
        if (grouped_totals[i] > 0 && bar_height == 0)
            bar_height = 1;

        for (int y = 0; y < bar_height; y++) {
            for (int w = 0; w < bar_width; w++) {
                mvwaddstr(chart_win, chart_bottom - y, current_x + w, "█");
            }
        }

        current_x += (bar_width + spacing);
    }

    arrfree(grouped_totals);
}
