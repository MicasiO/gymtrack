#ifndef CREATE_H
#define CREATE_H

#include <ctype.h>
#include <ncurses.h>
#include <ncurses/form.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "routine.h"

enum state {
    STATE_MENU,
    STATE_CREATE_TITLE,
    STATE_CREATE_WORKOUT,
    STATE_CREATE_CONTINUE,
    STATE_QUIT,
    STATE_START
};

enum state show_create_title(WINDOW* body_win, Routine** routines, Routine* draft);
enum state show_create_workout(WINDOW* body_win, Routine** routines, Routine* draft);
enum state show_create_continue(WINDOW* body_win, Routine** routines, Routine* draft);

#endif
