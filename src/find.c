#include "find.h"

#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "draw.h"
#include "editor.h"
#include "input.h"
#include "cursor.h"
#include "select.h"

void search(char *query) {
    bool found = false;

    for (int i = 0; i < editor.total_lines; i++) {
        char *pos = strstr(editor.lines[i], query);
        if (pos) {
            cursor.y = i - cursor.y_offset;
            cursor.x = pos - editor.lines[i] + editor.margin - cursor.x_offset;
            cursor.max_x = cursor.x;

            Point start_sel_point = {cursor.y + cursor.y_offset, 
                                     cursor.x + cursor.x_offset - editor.margin};
            Point end_sel_point = {cursor.y + cursor.y_offset,
                                   cursor.x + cursor.x_offset + strlen(query) - editor.margin + 1};

            place_cursor();
            clamp_cursor();

            set_selection(start_sel_point, end_sel_point);

            found = true;
            break;
        }
    }

    if (!found) {
        cancel_selection();
    }
}

void find(void) {
    int screen_height = getmaxy(stdscr);
    char *prompt = "Find: ";

    // Clear the last line and print the input prompt
    move(getmaxy(stdscr) - 1, 0);
    clrtoeol();
    mvprintw(screen_height - 1, 0, "%s", prompt);

    curs_set(1);

    char input[32] = {0};
    int pos = 0;

    int ch;

    // Read characters until Enter is pressed
    while ((ch = wgetch(stdscr)) != '\n') {
        // Redraw editor and print input line
        draw_editor();
        mvprintw(screen_height - 1, 0, "%s", prompt);

        if (ch == CTRL('q')) {
            exit_editor();
        }

        if (ch == KEY_ESCAPE) {
            return;
        }

        // Handle printable characters
        if ((ch == KEY_BACKSPACE || ch == KEY_DELETE || ch == '\b') && pos > 0) {
            input[--pos] = '\0';
            mvprintw(screen_height - 1, strlen(prompt), "%s", input);

            if (strlen(input) > 0) search(input);
            else cancel_selection();

            move(screen_height - 1, strlen(prompt) + pos);
        }

        // Handle backspace
        if (ch >= ' ' && ch < KEY_DELETE && pos < (int)sizeof(input) - 1) {
            input[pos++] = ch;
            mvprintw(screen_height - 1, strlen(prompt), "%s", input);

            if (strlen(input) > 0) search(input);
            else cancel_selection();

            move(screen_height - 1, strlen(prompt) + pos);
        }
    }
}

void find_replace(void) {

}
