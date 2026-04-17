#ifndef MENU_H
#define MENU_H

#include <ncurses.h>
#include "utils.h"

enum state show_main_menu(AppState* app_state);
void select_workout(AppState* app_state, enum state* next_state, const char ch, int curr_idx);

enum state show_workout_menu(AppState* app_state);

#endif
