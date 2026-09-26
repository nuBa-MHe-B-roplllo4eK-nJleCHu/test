// подключаем библиотеки
#include <ncurses.h> /* библиотека для интерактивных текстовых пользовательсих интерфейсов 
                        (на её основе работают vim и emacs) */
#include <stdio.h> // standart input output, стандартный ввод и вывод

// задаем константы
#define ROWS 25 // строки
#define COLS 80 // колонки
#define MIN_DELAY 20 // минимальная задержка
#define MAX_DELAY 200 // максимальная задержка
#define STEP_DELAY 20 // шаг при измненении задержки
#define DEFAULT_DELAY 100 // изначальная задержка

// объявляем прототипы функций
int input_field(int grid[ROWS][COLS]); 
int count_neighbors(const int grid[ROWS][COLS], int row, int col);
void step_simulation(const int src[ROWS][COLS], int dst[ROWS][COLS]);
void copy_grid(const int src[ROWS][COLS], int dst[ROWS][COLS]);
void render(const int grid[ROWS][COLS], int delay_ms);
int update_speed(int key, int current_delay);
void run_game(int grid[ROWS][COLS]);

/*
Функция main выступает лишь диспетчером. У неё 3 задачи:
1) Вылелить память под поле;
2) Проинициализировать изначальное состояние (для этого она вызывает функцию imput_field,
   которая читает текстовый файл, переданный в программу при запуске);
3) Вернуть ошибку, если функция input_field не смогла корректно прочитать файл, или
   запустить игру, если функция input_field всё корректно прочитала
*/
int main(void) { // main ничего не принимает в аргументах и возвращает целое число.
    int status = 0; // Переменная состояния, так называемый флаг.
    int grid[ROWS][COLS]; // Создаем двумерный массив целых чисел размером 25/80. 
                          // Мы объявили его внутри функции, поэтому он считается локальным
                          // Глобальные переменные запрещены в структурном программировании
    // Далее мы запускаем функцию input_field и передаём в неё АДРЕС ПЕРВОГО ЭЛЕМЕНТА двумерного массива
    // Функция считывает данные из файлика.
    if (input_field(grid)) {
        /* 
        Если input_field вернул 1, значит файл считан и выполняется этот блок.
        В линуксе всё есть файл.
        При старте каждой программы линукс создаёт для неё 3 структуры данных типа FILE: stdin, stdout, stderr
        (поток ввода, поток вывода, поток ошибок).
        При запуске мы писали ./game_of_life < pattern1.txt. То есть мы указывали стандартному потоку ввода 
        читать данные из файл pattern1.txt.
        Задача функции freopen - перепривязать уже открытый поток ввода stdin к новому файлу (к файлу терминала).
        В линуксе всё есть файл, поэтому текущий запущенный терминал - это файл /dev/tty.
        Синтаксис - freopen("имя_файла", "режим_работы", поток); (freopen = file re-open, переоткрыть файл) 
        Звездочка после FILE обозначает, что переменная tty является указателем. Она будет хранить не структуру
        данных, а адрес в оперативной памяти, на которую эта структура указывает.
        freopen возвращает именно указатель, если всё хорошо, а если не хорошо, то NULL
        */
        const FILE *tty = freopen("/dev/tty", "r", stdin); 
        if (tty != NULL) {
            // Если freopen вернул не NULL, а указатель на новый поток ввода, то запускаем функцию run_game
            run_game(grid);
        } else {
            // Если freopen вернул NULL, то меняем значение переменной статус
            status = 1;
        }
    } else {
        // Если input_field вернул 0 — произошла ошибка чтения и выполняется этот блок
        status = 1;
    }

    if (status == 1) {
        printf("n/a");
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
