#include <ncurses.h>
#include <stdio.h>

#define ROWS 25
#define COLS 80
#define MIN_DELAY 20
#define MAX_DELAY 200
#define STEP_DELAY 20
#define DEFAULT_DELAY 100

int input_field(int grid[ROWS][COLS]);
int count_neighbors(const int grid[ROWS][COLS], int row, int col);
void step_simulation(const int src[ROWS][COLS], int dst[ROWS][COLS]);
void copy_grid(const int src[ROWS][COLS], int dst[ROWS][COLS]);
void render(const int grid[ROWS][COLS], int delay_us);
int update_speed(int key, int current_delay);
void run_game(int grid[ROWS][COLS]);

int main(void) {
    int status = 0;
    int grid[ROWS][COLS];

    if (input_field(grid)) {
        FILE *tty = freopen("/dev/tty", "r", stdin);
        if (tty != NULL) {
            run_game(grid);
        } else {
            status = 1;
        }
    } else {
        status = 1;
    }

    return status;
}

int input_field(int grid[ROWS][COLS]) {
    int success = 1;
    for (int i = 0; i < ROWS && success; i++) {
        for (int j = 0; j < COLS && success; j++) {
            int val = 0;
            if (scanf("%d", &val) == 1) {
                grid[i][j] = (val != 0) ? 1 : 0;
            } else {
                success = 0;
            }
        }
    }
    return success;
}

int count_neighbors(const int grid[ROWS][COLS], int row, int col) {
    int count = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr != 0 || dc != 0) {
                int nr = (row + dr + ROWS) % ROWS;
                int nc = (col + dc + COLS) % COLS;
                count += grid[nr][nc];
            }
        }
    }
    return count;
}

void step_simulation(const int src[ROWS][COLS], int dst[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int neighbors = count_neighbors(src, i, j);
            if (src[i][j] == 1) {
                dst[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                dst[i][j] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
}

void copy_grid(const int src[ROWS][COLS], int dst[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

void render(const int grid[ROWS][COLS], int delay_ms) {
    clear();
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == 1) {
                mvaddch(i, j, 'O');
            } else {
                mvaddch(i, j, ' ');
            }
        }
    }
    mvprintw(ROWS, 0, "A: speed up | Z: slow down | SPACE: exit | Delay: %d ms",
             delay_ms);
    refresh();
}

int update_speed(int key, int current_delay) {
    int new_delay = current_delay;
    if (key == 'a' || key == 'A') {
        if (current_delay - STEP_DELAY >= MIN_DELAY) {
            new_delay = current_delay - STEP_DELAY;
        } else {
            new_delay = MIN_DELAY;
        }
    } else if (key == 'z' || key == 'Z') {
        if (current_delay + STEP_DELAY <= MAX_DELAY) {
            new_delay = current_delay + STEP_DELAY;
        } else {
            new_delay = MAX_DELAY;
        }
    }
    return new_delay;
}

void run_game(int grid[ROWS][COLS]) {
    int next_grid[ROWS][COLS];
    int delay_ms = DEFAULT_DELAY;
    int is_running = 1;

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    while (is_running) {
        timeout(delay_ms); 
        render(grid, delay_ms);

        int key = getch();
        if (key == ' ') {
            is_running = 0;
        } else {
            delay_ms = update_speed(key, delay_ms);
            step_simulation(grid, next_grid);
            copy_grid(next_grid, grid);
        }
    }

    endwin();
}
