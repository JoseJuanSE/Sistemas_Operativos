#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#define WIDTH 30
#define HEIGHT 10
int startx = 0;
int starty = 0;
char *choices[] = {
    "Opcion 1",
    "Opcion 2",
    "Opcion 3",
    "Opcion 4",
    "Salida",
};
int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);
int main()
{
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;
    initscr(); /* Start curses mode */
    clear();
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Tu terminal no soporta colores\n");
        exit(1);
    }
    start_color(); /* Start color */
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    noecho();
    cbreak(); /* Line buffering disabled. pass on everything */
    startx = (80 - WIDTH) / 2;
    starty = (24 - HEIGHT) / 2;
    menu_win = newwin(HEIGHT, WIDTH, starty, startx);
    keypad(menu_win, TRUE);
    mvprintw(0, 0, "Usa las flechas para ir arriba y abajo, Presiona entrer para escojer una opcion");
    refresh();
    print_menu(menu_win, highlight);
    while (1)
    {
        c = wgetch(menu_win);
        switch (c)
        {
        case KEY_UP:
            if (highlight == 1)
                highlight = n_choices;
            else
                --highlight;
            break;
        case KEY_DOWN:
            if (highlight == n_choices)
                highlight = 1;
            else
                ++highlight;
            break;
        case 10:
            choice = highlight;
            break;
        default:
            mvprintw(24, 0, "El caracter presionado es = %3d Ojala sea impresa como: '%c'", c, c);
            refresh();
            break;
        }
        print_menu(menu_win, highlight);
        if (choice != 0) /* User did a choice come out of the infinite loop */
            break;
    }
    mvprintw(23, 0, "Tu opcion elegida %d con opcion de cadena %s\n", choice, choices[choice - 1]);
    clrtoeol();
    refresh();
    attroff(COLOR_PAIR(1));
    endwin();
    return 0;
}
void print_menu(WINDOW *menu_win, int highlight)
{
    int x, y, i;
    x = 2;
    y = 2;
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i)
    {
        if (highlight == i + 1) /* High light the present choice */
        {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}
