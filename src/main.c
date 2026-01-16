#include <ctype.h>
#include <stdio.h>
#include <ncurses.h>
#include <dirent.h>
#define TUM_IMPLEMENTATION
#include "tum.h"

#define NCURSES_TIMEOUT 1000
#define PROC "/proc"

#define QUICK_INFO_BAR_HEIGHT 3
#define PROCESS_LIST_WINDOW_HEADER_HEIGHT 2

#define COLOR_PAIR_NORMAL 1
#define COLOR_PAIR_INFOBAR 2

int draw_quickinfo_bar(WINDOW *quickinfo_bar_window, double cpu_load_percentage);
int draw_proccess_list(WINDOW *process_list_window, int scroll_offset);

int main() {

    bool tum_running = true;
    int process_list_scroll_offset = 0;
    int curs_y = 2;
    int curs_x = 0;

    initscr();

    WINDOW *quickinfo_bar_window = newwin(QUICK_INFO_BAR_HEIGHT, COLS, LINES - QUICK_INFO_BAR_HEIGHT, 0);
    if (!quickinfo_bar_window) {
      endwin();
      fprintf(stderr, "FATAL: could not initialize quickinfo_bar_window!");
      return -1;
    }

    WINDOW *process_list_window = newwin(LINES - QUICK_INFO_BAR_HEIGHT, COLS, 0, 0);
    if (!process_list_window) {
        endwin();
        fprintf(stderr, "FATAL: could not initialize process_list_window!");
        return -1;
    }

    if (has_colors()) {
        start_color();
        init_pair(COLOR_PAIR_NORMAL, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_PAIR_INFOBAR, COLOR_BLACK, COLOR_WHITE);

        wbkgdset(quickinfo_bar_window, COLOR_PAIR(COLOR_PAIR_INFOBAR));
    }

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
        draw_proccess_list(process_list_window, process_list_scroll_offset);

        int ch = getch();
        switch (ch) {
            case 'q':
                tum_running = false;
                break;
            case 'j':
                if ((curs_y >= PROCESS_LIST_WINDOW_HEADER_HEIGHT) && (curs_y < LINES - QUICK_INFO_BAR_HEIGHT - 1 ) ) {
                    curs_y++;
                } else if (curs_y == LINES - QUICK_INFO_BAR_HEIGHT - 1) {
                   process_list_scroll_offset++;
                }
                break;
            case 'k':
                if (curs_y > PROCESS_LIST_WINDOW_HEADER_HEIGHT) {
                    curs_y--;
                } else if ((curs_y == PROCESS_LIST_WINDOW_HEADER_HEIGHT) && process_list_scroll_offset > 0) {
                    process_list_scroll_offset--;
                }
                break;
            case KEY_RESIZE:
                mvwin(quickinfo_bar_window, LINES - QUICK_INFO_BAR_HEIGHT , 0);
                wresize(process_list_window, LINES - QUICK_INFO_BAR_HEIGHT, COLS);
                curs_y = 2;
                curs_x = 0;
                break;
        }

        move(curs_y, curs_x);
        wrefresh(quickinfo_bar_window);
        wrefresh(process_list_window);
    }

    delwin(quickinfo_bar_window);
    delwin(process_list_window);
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

    return 0;
}

int draw_proccess_list(WINDOW *process_list_window, int scroll_offset) {
    DIR *proc = opendir(PROC);
    struct dirent *proc_child;

    int line_counter = 0;
    int remaining_free_lines = LINES - QUICK_INFO_BAR_HEIGHT;
    int x = 0;
    int y = 2;

    wclear(process_list_window);


    wprintw(process_list_window, "%-8s|%-16.16s|%-64.64s|", "   PID", "     PName", "                          cmdline");
    mvwhline(process_list_window, 1, 0, 0, COLS);

    while ((proc_child = readdir(proc)) && remaining_free_lines > 0) {

      if (!isdigit(proc_child->d_name[0])) {
        continue;
      }

      if (line_counter >= scroll_offset) {
        mvwprintw(process_list_window, y++, x, "%8s %-16.16s|%-64.64s|",
                  proc_child->d_name,
                  process_get_name(proc_child->d_name),
                  process_get_cmdline(proc_child->d_name));
        remaining_free_lines--;
      }

      line_counter++;
    }

    return 0;
}
