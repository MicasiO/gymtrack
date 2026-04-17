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
    STATE_QUIT,
    STATE_START
};

typedef struct AppState {
    Routine draft;     // draft for routine creation
    Routine* current;  // current viewed routine
    Routine* routines;
    WINDOW* body_win;
} AppState;

void die(const char* s);
int is_str_empty(char* str);
void serialize(Routine* routines);
void deserialize(Routine** routines);
void print_log(const char* s, ...);

#endif
