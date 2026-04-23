#ifndef ACTIVE_H
#define ACTIVE_H

#include "utils.h"

enum state show_active(AppState* app_state);
enum state show_active_sets(AppState* app_state);
enum state show_active_reps(AppState* app_state);

enum state show_finish_exercise(AppState* app_state);
enum state show_finish_routine(AppState* app_state);

void draw_formatted_reps(WINDOW* win, int sets, int* reps);

#endif
