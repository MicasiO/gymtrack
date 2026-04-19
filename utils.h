#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>
#include "routine.h"

enum state {
    STATE_MENU_MAIN,
    STATE_MENU_WORKOUT,
    STATE_CREATE_TITLE,
    STATE_CREATE_WORKOUT,
    STATE_CREATE_CONTINUE,
    STATE_ACTIVE,
    STATE_ACTIVE_CONTINUE,
    STATE_ACTIVE_DONE,
    STATE_QUIT
};

#define WIN_WIDTH 48
#define WIN_HEIGHT 21

typedef struct AppState {
    Routine draft;  // draft for routine creation
    Routine* routines;
    CurrentRoutine* current;  // active routine
    CurrentRoutine* history;  // previously done rouines;
    WINDOW* body_win;
} AppState;

void init_app_state(AppState* app_state);

void die(const char* s);
int is_str_empty(char* str);

void serialize_routines(Routine* routines);
void deserialize_routines(Routine** routines);

void serialize_history(CurrentRoutine* routines);
void deserialize_history(CurrentRoutine** routines);

void serialize_all(Routine* routines);
void deserialize_all(Routine** routines);

void print_log(const char* s, ...);
void draw_logo(WINDOW* win, int start_y, int start_x);

#endif
