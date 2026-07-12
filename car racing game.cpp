/* ============================================================
 *  2D CAR RACING GAME  (Windows console version)
 *  Works with Dev-C++ and VS Code (using the MinGW compiler)
 *
 *  Features: player controls, obstacle generation,
 *            collision detection, score tracking
 *
 *  Build (Dev-C++): just open the file and press F11 (Compile & Run)
 *  Build (VS Code):  gcc car_race_win.c -o car_race_win.exe
 *  Run:              car_race_win.exe
 * ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>      /* _kbhit(), _getch()  -- Windows only     */
#include <windows.h>    /* Sleep(), SetConsoleCursorPosition()     */
#include <time.h>

#define WIDTH        20   /* road width in characters   */
#define HEIGHT       15   /* road height in characters  */
#define MAX_OBST      5   /* obstacles on screen at once*/

typedef struct {
    int x, y;      /* column, row of the obstacle */
    int active;    /* is this slot in use?        */
} Obstacle;

int carX;                       /* player's column on the road   */
Obstacle obstacles[MAX_OBST];
int score = 0;
int speed = 200;                /* ms delay between frames        */

/* Move the console cursor to (x,y) so we can redraw without flicker */
void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hide_cursor(void) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info = { 100, FALSE };
    SetConsoleCursorInfo(h, &info);
}

/* Spawn one obstacle at the top of the road in a free lane/slot */
void spawn_obstacle(void) {
    for (int i = 0; i < MAX_OBST; i++) {
        if (!obstacles[i].active) {
            obstacles[i].active = 1;
            obstacles[i].x = rand() % WIDTH;
            obstacles[i].y = 0;
            return;
        }
    }
}

/* Move every active obstacle down by one row; free it if off-screen */
void update_obstacles(void) {
    for (int i = 0; i < MAX_OBST; i++) {
        if (obstacles[i].active) {
            obstacles[i].y++;
            if (obstacles[i].y >= HEIGHT) obstacles[i].active = 0;
        }
    }
}

/* Check whether the player's car occupies the same cell as an obstacle */
int check_collision(void) {
    for (int i = 0; i < MAX_OBST; i++) {
        if (obstacles[i].active &&
            obstacles[i].y == HEIGHT - 1 &&
            obstacles[i].x == carX) {
            return 1;
        }
    }
    return 0;
}

/* Draw the whole road, obstacles, player car and score in one pass */
void draw_board(void) {
    gotoxy(0, 0);

    for (int y = 0; y < HEIGHT; y++) {
        putchar('|');
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            /* draw player row */
            if (y == HEIGHT - 1 && x == carX) {
                putchar('A');
                printed = 1;
            }

            /* draw any obstacle at this cell */
            if (!printed) {
                for (int i = 0; i < MAX_OBST; i++) {
                    if (obstacles[i].active && obstacles[i].y == y && obstacles[i].x == x) {
                        putchar('X');
                        printed = 1;
                        break;
                    }
                }
            }

            if (!printed) putchar(' ');
        }
        printf("|   \n");
    }

    printf("Score: %d       \n", score);
    printf("Controls: A = left, D = right, Q = quit\n");
}

int main(void) {
    srand((unsigned) time(NULL));
    system("cls");
    hide_cursor();

    carX = WIDTH / 2;
    for (int i = 0; i < MAX_OBST; i++) obstacles[i].active = 0;

    int running = 1;
    int frame = 0;

    while (running) {
        /* --- handle input (non-blocking) --- */
        if (_kbhit()) {
            int ch = _getch();
            if ((ch == 'a' || ch == 'A') && carX > 0) carX--;
            if ((ch == 'd' || ch == 'D') && carX < WIDTH - 1) carX++;
            if (ch == 'q' || ch == 'Q') running = 0;
        }

        /* --- update game state --- */
        update_obstacles();
        if (frame % 6 == 0) spawn_obstacle();   /* spawn every few frames */
        score++;
        if (score % 100 == 0 && speed > 60) speed -= 10;  /* speed up over time */

        if (check_collision()) {
            gotoxy(0, HEIGHT + 2);
            printf("CRASH! Final score: %d           \n", score);
            printf("Press any key to exit...          \n");
            _getch();
            break;
        }

        /* --- draw --- */
        draw_board();

        Sleep(speed);
        frame++;
    }

    return 0;
}