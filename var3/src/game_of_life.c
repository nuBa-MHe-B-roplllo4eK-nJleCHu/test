#include <ncurses.h>
#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25
#define START_DELAY 200
#define MIN_DELAY 50
#define MAX_DELAY 1000
#define DELAY_STEP 50

int read_field(int field[HEIGHT][WIDTH]);
int read_cell_char(void);
void init_screen(void);
void game_loop(int field[HEIGHT][WIDTH]);
void draw_field(const int field[HEIGHT][WIDTH], int delay);
void change_speed(int key, int* delay);
void next_generation(int field[HEIGHT][WIDTH]);
int count_neighbors(const int field[HEIGHT][WIDTH], int y, int x);

int main(void) {
    int field[HEIGHT][WIDTH];
    int status = 0;
    if (read_field(field) != 0) {
        printf("n/a\n");
        status = 1;
    } else if (freopen("/dev/tty", "r", stdin) == NULL) {
        printf("n/a\n");
        status = 1;
    } else {
        init_screen();
        game_loop(field);
        endwin();
    }
    return status;
}

// Читает поле 25x80 из stdin: '*' - живая клетка, '.' - мёртвая. Возвращает 1 при ошибке.
int read_field(int field[HEIGHT][WIDTH]) {
    int error = 0;
    for (int y = 0; y < HEIGHT && !error; y++) {
        for (int x = 0; x < WIDTH && !error; x++) {
            int c = read_cell_char();
            if (c == '*' || c == '.') {
                field[y][x] = (c == '*') ? 1 : 0;
            } else {
                error = 1;
            }
        }
    }
    return error;
}

// Возвращает следующий символ из stdin, пропуская переводы строк.
int read_cell_char(void) {
    int c = getchar();
    while (c == '\n' || c == '\r') {
        c = getchar();
    }
    return c;
}

void init_screen(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
}

void game_loop(int field[HEIGHT][WIDTH]) {
    int delay = START_DELAY;
    int running = 1;
    while (running) {
        draw_field(field, delay);
        timeout(delay);
        int key = getch();
        if (key == ' ') {
            running = 0;
        } else {
            change_speed(key, &delay);
            next_generation(field);
        }
    }
}

void draw_field(const int field[HEIGHT][WIDTH], int delay) {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            mvaddch(y, x, field[y][x] ? '#' : '.');
        }
    }
    mvprintw(HEIGHT, 0, "Delay: %d ms | A - faster, Z - slower, Space - exit", delay);
    refresh();
}

// A - быстрее (задержка меньше), Z - медленнее (задержка больше).
void change_speed(int key, int* delay) {
    if ((key == 'a' || key == 'A') && *delay > MIN_DELAY) {
        *delay -= DELAY_STEP;
    }
    if ((key == 'z' || key == 'Z') && *delay < MAX_DELAY) {
        *delay += DELAY_STEP;
    }
}

void next_generation(int field[HEIGHT][WIDTH]) {
    int next[HEIGHT][WIDTH];
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int n = count_neighbors(field, y, x);
            next[y][x] = (n == 3 || (n == 2 && field[y][x] == 1)) ? 1 : 0;
        }
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = next[y][x];
        }
    }
}

// Поле замкнуто: соседи за краем берутся с противоположной стороны.
int count_neighbors(const int field[HEIGHT][WIDTH], int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int ny = (y + dy + HEIGHT) % HEIGHT;
            int nx = (x + dx + WIDTH) % WIDTH;
            count += field[ny][nx];
        }
    }
    count -= field[y][x];
    return count;
}
