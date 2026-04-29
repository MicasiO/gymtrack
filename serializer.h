#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "exercise.h"
#include "routine.h"

#ifdef __APPLE__
#define APP_DIR_NAME "Library/Application Support/gymtrack"
#elif __linux__
#define APP_DIR_NAME ".local/share/gymtrack"
#else
#define APP_DIR_NAME "gymtrack_data"
#endif

char* get_storage_path();

void serialize_routines(Routine* routines);
void deserialize_routines(Routine** routines);

void serialize_history(CurrentRoutine* routines);
void deserialize_history(CurrentRoutine** routines);

#endif
