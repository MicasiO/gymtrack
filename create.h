#ifndef CREATE_H
#define CREATE_H

#include "utils.h"

typedef struct AppState AppState;

enum state show_create_title(AppState* app_state);
enum state show_create_workout(AppState* app_state);
enum state show_create_continue(AppState* app_state);

#endif
