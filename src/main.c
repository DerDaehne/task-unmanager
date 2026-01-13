#include <stdio.h>
#include <ncurses.h>
#define TUM_IMPLEMENTATION
#include "tum.h"

#define NCURSES_TIMEOUT 1000

int draw_quickinfo_bar(WINDOW *quickinfo_bar_window, double cpu_load_percentage);

int main() {

    bool tum_running = true;

    initscr();
    WINDOW *quickinfo_bar_window = newwin(3, COLS, LINES-3, 0);

    if (!quickinfo_bar_window) {
      endwin();
      fprintf(stderr, "ERROR: could not initialize quickinfo_bar_window!");
      return -1;
    }

    wprintw(stdscr, "This Window has %d lines and %d cols.\n", LINES, COLS);
    wrefresh(stdscr);

    timeout(NCURSES_TIMEOUT);
    noecho();

    double current_total_jiffies = 0, last_total_jiffies = 0,
        current_cpu_usage = 0, last_cpu_usage = 0, cpu_load_percentage = 0;

    while (tum_running) {
        last_total_jiffies = current_total_jiffies;
        current_total_jiffies = cpu_get_total_time();

        last_cpu_usage = current_cpu_usage;
        current_cpu_usage = cpu_get_busy_time();

        if (current_total_jiffies - last_total_jiffies > 0) {
            cpu_load_percentage =  100 * (current_cpu_usage - last_cpu_usage) / (current_total_jiffies - last_total_jiffies);
        }

        draw_quickinfo_bar(quickinfo_bar_window, cpu_load_percentage);

        int ch = getch();
        if (ch == 'q') {
            tum_running = false;
        }
    }



    delwin(quickinfo_bar_window);
    endwin();

    return 0;
}


int draw_quickinfo_bar(WINDOW *quickinfo_bar_window, double cpu_load_percentage) {
    int mem_total = 0;
    char mem_unit[16] = "";
    int x = 1;
    int y = 1;

    if (mem_get_measurement("MemTotal:", &mem_total, mem_unit) != 0) {
        endwin();
        fprintf(stderr, "ERROR: could not get Memory Measurement!\n");
        return -1;
    }

    wclear(quickinfo_bar_window);

    mvwhline(quickinfo_bar_window, 0, 0, 0, COLS);
    mvwprintw(quickinfo_bar_window, y++, x, "This system has: %d %s of total memory\n", mem_total, mem_unit);
    mvwprintw(quickinfo_bar_window, y++, x, "CPU load: %f %%", cpu_load_percentage);

    wrefresh(quickinfo_bar_window);

    return 0;
}
