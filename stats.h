#ifndef STATS_H
#define STATS_H
#include "routine.h"
#include "utils.h"

enum state show_workout_stats(AppState* app_state);

// TODO: more stats
void display_title(WINDOW* win, char* title);
void display_exercise_count(WINDOW* win, long count);
void display_average_duration(WINDOW* win, CurrentRoutine** history_ptr);
void display_average_completion(WINDOW* win, CurrentRoutine** history_ptr);

enum state show_exercise_stats(AppState* app_state);

void display_exercise_chart(WINDOW* win,
                            WINDOW* chart_win,
                            int chart_h,
                            int chart_y,
                            CurrentExercise** history_ptr);
#endif
