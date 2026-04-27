#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>
#include "exercise.h"
#include "routine.h"

enum state {
    STATE_NULL,
    STATE_MENU_MAIN,
    STATE_MENU_WORKOUT,
    STATE_CREATE_TITLE,
    STATE_CREATE_WORKOUT,
    STATE_CREATE_CANCEL_EX,
    STATE_CREATE_CONTINUE,
    STATE_ACTIVE,
    STATE_ACTIVE_CONTINUE,
    STATE_ACTIVE_SETS,
    STATE_ACTIVE_REPS,
    STATE_ACTIVE_DONE,
    STATE_ACTIVE_FINISHED,
    STATE_QUIT
};

#define WIN_WIDTH 55
#define WIN_HEIGHT 21

typedef struct {
    struct timespec start_time;
    struct timespec stop_time;
    bool is_running;
} Stopwatch;

typedef struct AppState {
    Routine draft;              // draft for routine creation
    Routine* routines;          // routine templates
    CurrentRoutine* current;    // active routine
    CurrentExercise ex_backup;  // backup exercise if user cancels exercise
    CurrentRoutine* history;    // previously done rouines;
    WINDOW* body_win;           //
    Stopwatch* stopwatch;       //
    int reps_idx;               // counter for entering reps for exercises
} AppState;

void init_app_state(AppState* app_state);

void die(const char* s);
int is_str_empty(char* str);

void free_app_state(AppState* app_state);

void serialize_routines(Routine* routines);
void deserialize_routines(Routine** routines);

void serialize_history(CurrentRoutine* routines);
void deserialize_history(CurrentRoutine** routines);

void serialize_all(Routine* routines);
void deserialize_all(Routine** routines);

void print_log(const char* s, ...);
void draw_logo(WINDOW* win, int start_y, int start_x);

void start_stopwatch(Stopwatch* sw);
void stop_stopwatch(Stopwatch* sw);
double get_stopwatch(Stopwatch* sw);

void generate_uuid(char* buf);

#endif
