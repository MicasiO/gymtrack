#ifndef FORM_H_
#define FORM_H_

#include <ncurses.h>
#include <ncurses/form.h>
#include "utils.h"

enum field_type { FIELD_STRING, FIELD_NUMBER, FIELD_CHECKBOX };

typedef struct Field {
    FIELD* raw_field;
    WINDOW* win;
    char* title;
    enum field_type type;
} Field;

typedef struct Form {
    WINDOW* win;
    WINDOW* sub_win;
    FORM* raw_form;
    Field* fields;
    char* title;
    int x, y;
} Form;

void init_form(WINDOW* win, WINDOW* sub_win, Form* form, int y, int x);
void add_field_form(Form* form, const char* title, enum field_type type, int cap);
void show_form(Form* form);
void handle_input_form(Form* form,
                       enum state* state_ptr,
                       enum state prev_state,
                       enum state next_state);

char* get_field_value(Field* field);

void delete_form(Form* form);

#endif
