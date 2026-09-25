#include <ncurses.h>
#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25
#define START_SPEED 200
#define MIN_SPEED 50
#define MAX_SPEED 1000
#define SPEED_STEP 50

int input_field(int field[HEIGHT][WIDTH], const char *filename);
void print_field(int field[HEIGHT][WIDTH]);
int count_neighbors(int field[HEIGHT][WIDTH], int row, int col);
void make_generation(int field[HEIGHT][WIDTH], int next_field[HEIGHT][WIDTH]);
void copy_field(int field[HEIGHT][WIDTH], int next_field[HEIGHT][WIDTH]);
int handle_input(int *speed);

int main(int argc, char *argv[]) {
    int field[HEIGHT][WIDTH];
    int next_field[HEIGHT][WIDTH];
    int speed = START_SPEED;
    int game_over = 0;

    if (argc == 2 && input_field(field, argv[1]) == 1) {
        initscr();
        noecho();
        cbreak();
        curs_set(0);
        nodelay(stdscr, TRUE);

        while (game_over == 0) {
            print_field(field);
            game_over = handle_input(&speed);

            if (game_over == 0) {
                make_generation(field, next_field);
                copy_field(field, next_field);
                napms(speed);
            }
        }

        endwin();
    } else {
        printf("n/a");
    }

    return 0;
}

int input_field(int field[HEIGHT][WIDTH], const char *filename) {
    FILE *file = fopen(filename, "r");
    int result = 1;
    int row = 0;
    int col = 0;
    int symbol;

    if (file == NULL) {
        result = 0;
    }

    while (row < HEIGHT && result == 1) {
        while (col < WIDTH && result == 1) {
            symbol = fgetc(file);

            if (symbol == '0' || symbol == '1') {
                field[row][col] = symbol - '0';
                col++;
            } else if (symbol != '\n' && symbol != '\r' && symbol != ' ' && symbol != '\t') {
                result = 0;
            } else if (symbol == EOF) {
                result = 0;
            }
        }

        if (result == 1) {
            row++;
            col = 0;
        }
    }

    if (result == 1) {
        symbol = fgetc(file);

        while (symbol == '\n' || symbol == '\r' || symbol == ' ' || symbol == '\t') {
            symbol = fgetc(file);
        }

        if (symbol != EOF) {
            result = 0;
        }
    }

    if (file != NULL) {
        fclose(file);
    }

    return result;
}

void print_field(int field[HEIGHT][WIDTH]) {
    int row;
    int col;

    clear();

    for (row = 0; row < HEIGHT; row++) {
        for (col = 0; col < WIDTH; col++) {
            if (field[row][col] == 1) {
                mvaddch(row, col, 'O');
            } else {
                mvaddch(row, col, ' ');
            }
        }
    }

    refresh();
}

int count_neighbors(int field[HEIGHT][WIDTH], int row, int col) {
    int count = 0;
    int dr;
    int dc;
    int neighbor_row;
    int neighbor_col;

    for (dr = -1; dr <= 1; dr++) {
        for (dc = -1; dc <= 1; dc++) {
            if (dr != 0 || dc != 0) {
                neighbor_row = (row + dr + HEIGHT) % HEIGHT;
                neighbor_col = (col + dc + WIDTH) % WIDTH;
                count += field[neighbor_row][neighbor_col];
            }
        }
    }

    return count;
}

void make_generation(int field[HEIGHT][WIDTH], int next_field[HEIGHT][WIDTH]) {
    int row;
    int col;
    int neighbors;

    for (row = 0; row < HEIGHT; row++) {
        for (col = 0; col < WIDTH; col++) {
            neighbors = count_neighbors(field, row, col);

            if (field[row][col] == 1) {
                next_field[row][col] = (neighbors == 2 || neighbors == 3);
            } else {
                next_field[row][col] = (neighbors == 3);
            }
        }
    }
}

void copy_field(int field[HEIGHT][WIDTH], int next_field[HEIGHT][WIDTH]) {
    int row;
    int col;

    for (row = 0; row < HEIGHT; row++) {
        for (col = 0; col < WIDTH; col++) {
            field[row][col] = next_field[row][col];
        }
    }
}

int handle_input(int *speed) {
    int key = getch();
    int game_over = 0;

    if (key == ' ') {
        game_over = 1;
    } else if (key == 'a' || key == 'A') {
        if (*speed > MIN_SPEED) {
            *speed -= SPEED_STEP;
        }
    } else if (key == 'z' || key == 'Z') {
        if (*speed < MAX_SPEED) {
            *speed += SPEED_STEP;
        }
    }

    return game_over;
}