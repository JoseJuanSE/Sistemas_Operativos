#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
void print_menu(WINDOW *menu_win, int highlight) {
    int x, y;
    x = 1;
    y = 1;
    box(menu_win, 0, 0);
    if (highlight == 1) {
        wattron(menu_win, A_REVERSE);
        mvwprintw(menu_win, y, x, "%s", "Sí");
        wattroff(menu_win, A_REVERSE);
    }
    else
        mvwprintw(menu_win, y, x, "%s", "Sí");
    x += 6;
    if (highlight == 2) {
        wattron(menu_win, A_REVERSE);
        mvwprintw(menu_win, y, x, "%s", "No");
        wattroff(menu_win, A_REVERSE);
    }
    else
        mvwprintw(menu_win, y, x, "%s", "No");
    wrefresh(menu_win);
}

void print_hack(WINDOW *win, int starty, int startx, int width, char *string) {
    int length, x, y, highlight = 1, choice = 0, c;
    float temp;
    if (win == NULL) win = stdscr;
    getyx(win, y, x);
    if (startx != 0) x = startx;
    if (starty != 0) y = starty;
    if (width == 0) width = 80;
    length = strlen(string);
    temp = (width - length) / 2;
    y -= 5;
    x = startx + (int)temp;
    mvwprintw(win, --y, x--, "%s", string);
    y += 2;
    refresh();
    int r = 1000000;
    for(int i = 0; i <= length + 1; i++) {
        mvwprintw(win, y, x++, "%c", '|');
        mvwprintw(win, y + 2, startx + (int)temp, "%s", "Descifrando...");
        refresh();
        r -= 45000;
        usleep(r);
    }
    y += 3;
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(2));
    mvwprintw(win, ++y, startx + (int)temp, "%s", "ACCESO CONCEDIDO");
    refresh();
    attroff(COLOR_PAIR(2));
    y++;
    attron(COLOR_PAIR(1));
    mvwprintw(win, ++y, startx + (int)temp, "%s", "AUTODESTRUIR:");
    refresh();
    y++;
    WINDOW *menu_win = newwin(3, 10, y, x);
    keypad(menu_win, TRUE);
    print_menu(menu_win, highlight);
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
        case KEY_RIGHT:
            if (highlight == 1)
                ++highlight;
            else
                --highlight;
            break;
        case KEY_LEFT:
            if (highlight == 2)
                --highlight;
            else
                ++highlight;
            break;
        case 10:
            choice = highlight;
            break;
        default:
            break;
        }
        print_menu(menu_win, highlight);
        if (choice != 0) /* User did a choice come out of the infinite loop */
            break;
    }
    refresh();
    attroff(COLOR_PAIR(1));
}
int main(int argc, char *argv[]) {
    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printf("La terminal no soporta colores\n");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    print_hack(stdscr, LINES / 2, 0, 0, "HACKEANDO EL SISTEMA");
    attroff(COLOR_PAIR(1));
    endwin();
}
