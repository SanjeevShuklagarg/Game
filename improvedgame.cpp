/* LANE RUNNER – Flickerless + Difficulty Modes (Dev-C++ Friendly)
   By ChatGPT
*/

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH_LANES 3
#define VHEIGHT 10
#define MAX_OBSTACLES 4
#define START_LIVES 3
#define HS_FILENAME "highscore.txt"

#define BUF_W 30
#define BUF_H 25

char SCREEN[BUF_H][BUF_W + 1]; // double buffer

typedef struct {
    int y;
    int x;
    int speed;
    int active;
} Obstacle;

/* Double buffer clear */
void clear_buffer() {
    for (int i = 0; i < BUF_H; i++) {
        for (int j = 0; j < BUF_W; j++)
            SCREEN[i][j] = ' ';
        SCREEN[i][BUF_W] = '\0';
    }
}

/* Print double buffer at once = no flickering */
void render_buffer() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0,0};
    SetConsoleCursorPosition(h, pos);

    for (int i = 0; i < BUF_H; i++)
        printf("%s\n", SCREEN[i]);
}

void set_black_bg() { system("color 0F"); }

int load_highscore() {
    FILE *f = fopen(HS_FILENAME, "r");
    int hs = 0;
    if (f) { fscanf(f, "%d", &hs); fclose(f); return hs; }
    f = fopen(HS_FILENAME, "w");
    if (f) { fprintf(f, "0\n"); fclose(f); }
    return 0;
}

void save_highscore(int hs) {
    FILE *f = fopen(HS_FILENAME, "w");
    if (f) { fprintf(f, "%d\n", hs); fclose(f); }
}

void reset_obstacles(Obstacle o[]) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        o[i].active = 0;
        o[i].y = -1;
        o[i].x = 0;
        o[i].speed = 1;
    }
}

/* Draw game into buffer */
void draw_game(Obstacle obs[], int player_lane, int score, int lives, int speed, int highscore) {
    clear_buffer();

    // HUD
    sprintf(SCREEN[0], "Score:%d Lives:%d HS:%d Spd:%d", score, lives, highscore, speed);

    sprintf(SCREEN[1], "-----------------------------");
    sprintf(SCREEN[2], "|--- --- ---|");

    // Game area rows 3 to (2+VHEIGHT)
    for (int r = 0; r < VHEIGHT; r++) {
        int row = 3 + r;
        int placed = 0;

        for (int o = 0; o < MAX_OBSTACLES; o++) {
            if (obs[o].active && obs[o].y == r) {
                if (obs[o].x == 0)      sprintf(SCREEN[row], "| O         |");
                else if (obs[o].x == 1) sprintf(SCREEN[row], "|     O     |");
                else                    sprintf(SCREEN[row], "|        O  |");
                placed = 1;
                break;
            }
        }

        if (!placed) sprintf(SCREEN[row], "|           |");
    }

    // Player row
    int pr = 3 + VHEIGHT;
    if (player_lane == 0)      sprintf(SCREEN[pr], "| P         |");
    else if (player_lane == 1) sprintf(SCREEN[pr], "|     P     |");
    else                       sprintf(SCREEN[pr], "|        P  |");
}

/* Difficulty Menu */
int difficulty_menu() {
    clear_buffer();
    sprintf(SCREEN[3], "Select Difficulty:");
    sprintf(SCREEN[5], "1. EASY");
    sprintf(SCREEN[6], "2. NORMAL");
    sprintf(SCREEN[7], "3. HARD");
    sprintf(SCREEN[9], "Press (1-3)");

    render_buffer();

    while (1) {
        int c = getch();
        if (c >= '1' && c <= '3') return c - '0';
    }
}

void spawn_obstacles(Obstacle o[], int chance) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!o[i].active && (rand() % 100) < chance) {
            o[i].active = 1;
            o[i].y = 0;
            o[i].x = rand() % WIDTH_LANES;
            o[i].speed = 1 + rand() % 2;
        }
    }
}

void update_obstacles(Obstacle o[]) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (o[i].active) {
            o[i].y += o[i].speed;
            if (o[i].y > VHEIGHT) {
                o[i].active = 0;
                o[i].y = -1;
            }
        }
    }
}

/* Main Game */
int run_game(int *final_score, int *highscore) {
    Obstacle obs[MAX_OBSTACLES];
    reset_obstacles(obs);

    int difficulty = difficulty_menu();

    int speed;
    int spawnRate;

    // difficulty setup
    if (difficulty == 1) { speed = 180; spawnRate = 9; }
    else if (difficulty == 2) { speed = 140; spawnRate = 12; }
    else { speed = 100; spawnRate = 16; }

    int lane = 1;
    int score = 0;
    int lives = START_LIVES;
    int f = 0;

    while (1) {
        if (_kbhit()) {
            int ch = getch();
            if (ch == 224) {
                ch = getch();
                if (ch == 75 && lane > 0) lane--;
                if (ch == 77 && lane < 2) lane++;
            }
        }

        spawn_obstacles(obs, spawnRate);
        draw_game(obs, lane, score, lives, speed, *highscore);
        render_buffer();

        Sleep(speed);
        update_obstacles(obs);

        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (obs[i].active && obs[i].y >= VHEIGHT) {
                if (obs[i].x == lane) {
                    lives--;
                    obs[i].active = 0;
                    obs[i].y = -1;
                    if (lives <= 0) {
                        *final_score = score;
                        return 1;
                    }
                } else {
                    score += 5;
                    obs[i].active = 0;
                    obs[i].y = -1;
                }
            }
        }

        f++;
        if (f % 6 == 0) score++;

        if (score % 100 == 0 && score > 0) {
            if (speed > 60) speed -= 5;
            spawnRate += 1;
            score++;
        }
    }
}

/* MENU */
int menu(int highscore) {
    clear_buffer();
    sprintf(SCREEN[3], "=== LANE RUNNER ===");
    sprintf(SCREEN[5], "1. Start Game");
    sprintf(SCREEN[6], "2. High Score");
    sprintf(SCREEN[7], "3. Exit");
    sprintf(SCREEN[9], "High Score: %d", highscore);
    sprintf(SCREEN[11], "Press (1-3)");

    render_buffer();

    while (1) {
        int c = getch();
        if (c >= '1' && c <= '3') return c - '0';
    }
}

int main() {
    srand((unsigned)time(NULL));
    set_black_bg();

    int hs = load_highscore();

    while (1) {
        int m = menu(hs);

        if (m == 1) {
            int final_score;
            int r = run_game(&final_score, &hs);

            clear_buffer();
            sprintf(SCREEN[3], "=== GAME OVER ===");
            sprintf(SCREEN[5], "Your Score: %d", final_score);

            if (final_score > hs) {
                hs = final_score;
                save_highscore(hs);
                sprintf(SCREEN[7], "NEW HIGH SCORE!");
            } else {
                sprintf(SCREEN[7], "High Score: %d", hs);
            }

            sprintf(SCREEN[9], "Press Y to Retry, N to Quit");

            render_buffer();

            while (1) {
                int c = getch();
                if (c == 'y' || c == 'Y') break;
                if (c == 'n' || c == 'N') goto exit_game;
            }
        }
        else if (m == 2) {
            clear_buffer();
            sprintf(SCREEN[3], "High Score: %d", hs);
            sprintf(SCREEN[5], "Press any key...");
            render_buffer();
            getch();
        }
        else break;
    }

exit_game:
    clear_buffer();
    sprintf(SCREEN[5], "Thanks for playing!");
    render_buffer();
    Sleep(800);
    return 0;
}
