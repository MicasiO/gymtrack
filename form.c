#include "form.h"
#include <ctype.h>
#include <ncurses.h>
#ifdef __linux__
#include <ncurses/form.h>
#else
#include <form.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "stb_ds.h"
#include "utils.h"

void init_form(WINDOW* win, WINDOW* sub_win, Form* form, int y, int x) {
    form->win = win;
    form->sub_win = sub_win;
    form->fields = NULL;
    form->raw_form = NULL;
    form->title = NULL;
    form->y = y;
    form->x = x;
}

void add_field_form(Form* form, const char* title, enum field_type type, int cap) {
    Field field;

    int field_y = form->y + (arrlen(form->fields) * 3);
    int field_x = form->x + strlen(title) + 2;

    field.raw_field = new_field(1, cap, field_y, field_x, 0, 0);
    field.title = (char*)title;
    field.type = type;
    field.win = derwin(form->win, 3, cap + 2, field_y, field_x);
    if (type == FIELD_CHECKBOX) {
        set_field_buffer(field.raw_field, 0, "[ ]");
    }

    arrput(form->fields, field);
}

void show_form(Form* form) {
    int fields_len = arrlen(form->fields);

    if (fields_len == 0) {
        form->raw_form = NULL;
        return;
    }

    FIELD** raw_fields = calloc(fields_len + 1, sizeof(FIELD*));
    pos_form_cursor(form->raw_form);

    for (int i = 0; i < fields_len; i++) {
        field_opts_off(form->fields[i].raw_field, O_AUTOSKIP);

        raw_fields[i] = form->fields[i].raw_field;
    }
    wrefresh(form->sub_win);
    raw_fields[fields_len] = NULL;

    form->raw_form = new_form(raw_fields);

    set_form_win(form->raw_form, form->win);
    set_form_sub(form->raw_form, form->sub_win);

    post_form(form->raw_form);

    for (int i = 0; i < fields_len; i++) {
        mvwprintw(form->sub_win, form->y + (i * 3), form->x, "%s", form->fields[i].title);

        if (form->fields[i].type != FIELD_CHECKBOX) {
            box(form->fields[i].win, 0, 0);
        }
    }

    wrefresh(form->win);
    wrefresh(form->sub_win);
}

void handle_input_form(Form* form,
                       enum state* state_ptr,
                       enum state prev_state,
                       enum state next_state) {
    bool running = true;
    int ch;

    pos_form_cursor(form->raw_form);
    wrefresh(form->sub_win);
    while (running && (ch = wgetch(form->sub_win)) != KEY_F(1)) {
        switch (ch) {
            case KEY_DOWN:
                form_driver(form->raw_form, REQ_NEXT_FIELD);
                form_driver(form->raw_form, REQ_END_LINE);
                break;
            case KEY_UP:
                form_driver(form->raw_form, REQ_PREV_FIELD);
                form_driver(form->raw_form, REQ_END_LINE);
                break;
            case KEY_BTAB:  // shift + tab
                form_driver(form->raw_form, REQ_PREV_FIELD);
                form_driver(form->raw_form, REQ_END_LINE);
                break;
            case 9:  // tab
                form_driver(form->raw_form, REQ_NEXT_FIELD);
                form_driver(form->raw_form, REQ_END_LINE);
                break;
            case KEY_BACKSPACE:
            case 127:  // delete
            case 8:
                form_driver(form->raw_form, REQ_DEL_PREV);
                break;
            case KEY_LEFT:
                form_driver(form->raw_form, REQ_PREV_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(form->raw_form, REQ_VALIDATION);

                int cursor_col = form->raw_form->curcol;

                char* buf = field_buffer(current_field(form->raw_form), 0);
                int last_char_pos = strlen(buf);
                while (last_char_pos > 0 && buf[last_char_pos - 1] == ' ') {
                    last_char_pos--;
                }

                if (cursor_col < last_char_pos) {
                    form_driver(form->raw_form, REQ_NEXT_CHAR);
                }

                break;
            case 27:  // esc
                *state_ptr = prev_state;
                running = false;
                break;
            case 10:  // enter
                bool empty = false;
                form_driver(form->raw_form, REQ_VALIDATION);

                for (int i = 0; i < arrlen(form->fields); i++) {
                    char* buf = field_buffer(form->fields[i].raw_field, 0);
                    if (is_str_empty(buf)) {
                        empty = true;
                        break;
                    }
                }

                if (!empty) {
                    *state_ptr = next_state;
                    running = false;
                }
                break;
            default:
                bool reject = false;
                for (int i = 0; i < arrlen(form->fields); i++) {
                    Field f = form->fields[i];
                    if (current_field(form->raw_form) == f.raw_field && f.type == FIELD_NUMBER) {
                        if (!isdigit(ch)) {
                            beep();
                            reject = true;
                            break;
                        }
                    }
                }

                if (!reject) {
                    form_driver(form->raw_form, ch);
                }
                break;
        }
        pos_form_cursor(form->raw_form);
        wrefresh(form->sub_win);
    }
}

char* get_field_value(Field* field) {
    if (!field || !field->raw_field)
        return NULL;

    char* raw_buf = field_buffer(field->raw_field, 0);
    if (!raw_buf) {
        return NULL;
    }

    char* clean_str = strdup(raw_buf);

    int len = strlen(clean_str);
    while (len > 0 && isspace((unsigned char)clean_str[len - 1])) {
        clean_str[len - 1] = '\0';
        len--;
    }

    return clean_str;
}

void delete_form(Form* form) {
    if (form->raw_form != NULL) {
        unpost_form(form->raw_form);

        FIELD** fields_arr = form_fields(form->raw_form);
        free_form(form->raw_form);
        free(fields_arr);
    }

    for (int i = 0; i < arrlen(form->fields); i++) {
        delwin(form->fields[i].win);
        free_field(form->fields[i].raw_field);
    }

    arrfree(form->fields);
}
