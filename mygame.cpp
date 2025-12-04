#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_SNAKE_LENGTH 200

#define DIFF_EASY 120
#define DIFF_MEDIUM 80
#define DIFF_HARD 40
#define MIN_SLEEP 20


#define COLOR_BORDER 7   
#define COLOR_BG 0       
#define COLOR_SNAKE_HEAD 10  
#define COLOR_SNAKE_BODY 2   
#define COLOR_FOOD 12        
#define COLOR_TEXT 15        


enum { UP = 0, DOWN, LEFT, RIGHT };

typedef struct { int x, y; } Point;

typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    int direction;
    int grow; 
} Snake;

typedef struct { Point position; } Food;


Snake snake;
Food food;
int score = 0;
int highScore = 0;
char highScoreName[50] = "None";
char playerName[50] = "Player";
int baseSpeed = DIFF_MEDIUM; 


void initConsole();
void setCursorVisible(int visible);
void gotoxy(int x, int y);
void drawBorders();
void drawCell(int x, int y, char c, WORD attr);
void setupGame();
void generateFood();
void drawInitial();
void inputHandler(int *running);
void logic();
void gameOver();
void saveHighScore();
void loadHighScore();
void clearArea();
void showMenu();
int menu();
void howToPlay();
void showHighScoreScreen();
void beepEat();
void beepGameOver();
void maximizeConsole();

int main(void) {
    srand((unsigned int)time(NULL));

    initConsole();
    loadHighScore();
    maximizeConsole();
    setCursorVisible(0);

    while (1) {
        int choice = menu();
        if (choice == 1) { 
            setupGame();
            drawInitial();

            int running = 1;
            while (running) {
                inputHandler(&running);
                logic();
                
                int speed = baseSpeed - (snake.length / 2);
                if (speed < MIN_SLEEP) speed = MIN_SLEEP;
                Sleep(speed);
            }
            
        } else if (choice == 2) { 
            showHighScoreScreen();
        } else if (choice == 3) { 
            howToPlay();
        } else { 
            system("cls");
            setCursorVisible(1);
            return 0;
        }
    }
    return 0;
}



void initConsole() {

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

void maximizeConsole() {
    HWND console = GetConsoleWindow();
    if (console) {
        ShowWindow(console, SW_MAXIMIZE);

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        SMALL_RECT r;
        r.Left = 0;
        r.Top = 0;
        r.Right = WIDTH + 1;   
        r.Bottom = HEIGHT + 6; 
        COORD buf = { (SHORT)(r.Right + 1), (SHORT)(r.Bottom + 1) };
        SetConsoleScreenBufferSize(hOut, buf);
        SetConsoleWindowInfo(hOut, TRUE, &r);
    }
}

void setCursorVisible(int visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void drawCell(int x, int y, char c, WORD attr) {
   
    COORD pos = { (SHORT)(x + 1), (SHORT)(y + 1) };
    DWORD written;
    CHAR_INFO chi;
    chi.Char.AsciiChar = c;
    chi.Attributes = attr;
    SMALL_RECT sr = { pos.X, pos.Y, pos.X, pos.Y };
    COORD bufSize = { 1, 1 };
    COORD bufCoord = { 0, 0 };
    WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &chi, bufSize, bufCoord, &sr);
}



void saveHighScore() {
    FILE *f = fopen("highscore.txt", "w");
    if (!f) return;
    fprintf(f, "%s\n%d\n", highScoreName, highScore);
    fclose(f);
}

void loadHighScore() {
    FILE *f = fopen("highscore.txt", "r");
    if (!f) {
        highScore = 0;
        strcpy(highScoreName, "None");
        return;
    }
    char namebuf[100];
    int sc = 0;
    if (fgets(namebuf, sizeof(namebuf), f) != NULL) {
       
        namebuf[strcspn(namebuf, "\r\n")] = 0;
        strncpy(highScoreName, namebuf, sizeof(highScoreName)-1);
        highScoreName[sizeof(highScoreName)-1] = 0;
    }
    if (fscanf(f, "%d", &sc) == 1) highScore = sc;
    fclose(f);
}


int menu() {
    while (1) {
        system("cls");
        setCursorVisible(1);
        
        printf("========== SNAKE GAME ==========\n\n");
        printf("1. Start Game\n");
        printf("2. High Score\n");
        printf("3. How to Play\n");
        printf("4. Exit\n\n");
        printf("Choose (1-4): ");
        int c = 0;
        c = getch();
        putchar(c);
        printf("\n");
        if (c == '1') {
            
            printf("\nEnter your name (max 20 chars): ");
            fflush(stdout);
            
            scanf("%19s", playerName);
            
            int d = 0;
            while (1) {
                printf("\nSelect difficulty:\n");
                printf("1. Easy\n2. Medium\n3. Hard\n");
                printf("Choice (1-3): ");
                d = getch();
                putchar(d);
                printf("\n");
                if (d == '1') { baseSpeed = DIFF_EASY; break; }
                if (d == '2') { baseSpeed = DIFF_MEDIUM; break; }
                if (d == '3') { baseSpeed = DIFF_HARD; break; }
            }
            setCursorVisible(0);
            return 1;
        } else if (c == '2') {
            return 2;
        } else if (c == '3') {
            return 3;
        } else if (c == '4') {
            return 4;
        }
        
    }
}

void showHighScoreScreen() {
    system("cls");
    setCursorVisible(1);
    printf("------ High Score ------\n\n");
    printf("Name : %s\n", highScoreName);
    printf("Score: %d\n\n", highScore);
    printf("Press any key to return to menu...");
    getch();
    setCursorVisible(0);
}

void howToPlay() {
    system("cls");
    setCursorVisible(1);
    printf("------ How to Play ------\n\n");
    printf("- Use arrow keys to move the snake.\n");
    printf("- Eat food (*) to grow and gain +10 points.\n");
    printf("- Press 'P' to pause/resume.\n");
    printf("- Press 'Q' in-game to quit to menu.\n");
    printf("- Avoid hitting walls or your own body.\n\n");
    printf("Press any key to return to menu...");
    getch();
    setCursorVisible(0);
}



void drawBorders() {
    
    WORD attr = COLOR_BORDER;
    for (int i = 0; i < WIDTH + 2; i++) {
        gotoxy(i, 0);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attr);
        putchar('#');
    }
    
    for (int y = 1; y <= HEIGHT; y++) {
        gotoxy(0, y);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attr);
        putchar('#');
        gotoxy(WIDTH + 1, y);
        putchar('#');
    }
    
    for (int i = 0; i < WIDTH + 2; i++) {
        gotoxy(i, HEIGHT + 1);
        putchar('#');
    }
    
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR_TEXT);
    
    gotoxy(0, HEIGHT + 3);
    printf("Player: %s    Current Score: %d    Highest: %s - %d\n", playerName, score, highScoreName, highScore);
    gotoxy(0, HEIGHT + 4);
    printf("Controls: Arrow keys  |  P = Pause  |  Q = Quit to menu\n");
}

void drawInitial() {
    system("cls"); 
    drawBorders();

    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            drawCell(x, y, ' ', COLOR_BG);
        }
    }

   
    for (int i = 0; i < snake.length; i++) {
        if (i == 0)
            drawCell(snake.body[0].x, snake.body[0].y, 'O', COLOR_SNAKE_HEAD);
        else
            drawCell(snake.body[i].x, snake.body[i].y, 'o', COLOR_SNAKE_BODY);
    }

    
    drawCell(food.position.x, food.position.y, '*', COLOR_FOOD);

    
    gotoxy(0, HEIGHT + 3);
    printf("Player: %s    Current Score: %d    Highest: %s - %d      ", playerName, score, highScoreName, highScore);
}



void setupGame() {

    snake.length = 3;
    snake.body[0].x = WIDTH / 2;
    snake.body[0].y = HEIGHT / 2;
    snake.body[1].x = snake.body[0].x - 1;
    snake.body[1].y = snake.body[0].y;
    snake.body[2].x = snake.body[0].x - 2;
    snake.body[2].y = snake.body[0].y;
    snake.direction = RIGHT;
    snake.grow = 0;

    score = 0;

 
    generateFood();
}

void generateFood() {
    int valid = 0;
    while (!valid) {
        valid = 1;
        food.position.x = rand() % WIDTH;
        food.position.y = rand() % HEIGHT;
        for (int i = 0; i < snake.length; i++) {
            if (food.position.x == snake.body[i].x && food.position.y == snake.body[i].y) {
                valid = 0;
                break;
            }
        }
    }
}



void inputHandler(int *running) {

    if (_kbhit()) {
        int ch = _getch();
        if (ch == 0 || ch == 224) { 
            int arrow = _getch();
            switch (arrow) {
                case 72: 
                    if (snake.direction != DOWN) snake.direction = UP;
                    break;
                case 80: 
                    if (snake.direction != UP) snake.direction = DOWN;
                    break;
                case 75: 
                    if (snake.direction != RIGHT) snake.direction = LEFT;
                    break;
                case 77: 
                    if (snake.direction != LEFT) snake.direction = RIGHT;
                    break;
            }
        } else {
            
            if (ch == 'p' || ch == 'P') {
            
                gotoxy(0, HEIGHT + 5);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR_TEXT);
                printf("Paused. Press 'P' again to resume...");
                while (1) {
                    if (_kbhit()) {
                        int cc = _getch();
                        if (cc == 'p' || cc == 'P') {
            
                            gotoxy(0, HEIGHT + 5);
                            printf("                                      ");
                            break;
                        } else if (cc == 'q' || cc == 'Q') {
            
                            *running = 0;
                            return;
                        }
                    }
                    Sleep(50);
                }
            } else if (ch == 'q' || ch == 'Q') {
            
                *running = 0;
            }
        }
    }
}

void logic() {
    
    Point prevTail = snake.body[snake.length - 1];

    
    if (snake.length > 1) {
        for (int i = snake.length - 1; i > 0; i--) {
            snake.body[i] = snake.body[i - 1];
        }
    }

    
    switch (snake.direction) {
        case UP: snake.body[0].y--; break;
        case DOWN: snake.body[0].y++; break;
        case LEFT: snake.body[0].x--; break;
        case RIGHT: snake.body[0].x++; break;
    }

    
    if (snake.body[0].x < 0 || snake.body[0].x >= WIDTH ||
        snake.body[0].y < 0 || snake.body[0].y >= HEIGHT) {
        beepGameOver();
        gameOver();
        return;
    }

    
    for (int i = 1; i < snake.length; i++) {
        if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
            beepGameOver();
            gameOver();
            return;
        }
    }

    
    if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y) {
    
        score += 10;
        snake.grow += 1; 
        beepEat();

        
        if (score > highScore) {
            highScore = score;
            strncpy(highScoreName, playerName, sizeof(highScoreName)-1);
            highScoreName[sizeof(highScoreName)-1] = '\0';
            saveHighScore();
        }

        
        generateFood();

        
        if (snake.length < MAX_SNAKE_LENGTH) {
        
            if (snake.length + 1 <= MAX_SNAKE_LENGTH) {
                snake.body[snake.length] = prevTail;
                snake.length++;
            }
        }
        
        gotoxy(0, HEIGHT + 3);
        printf("Player: %s    Current Score: %d    Highest: %s - %d      ", playerName, score, highScoreName, highScore);
        
        drawCell(food.position.x, food.position.y, '*', COLOR_FOOD);
    } else {
        
        drawCell(prevTail.x, prevTail.y, ' ', COLOR_BG);
    }

    
    if (snake.length > 1) {
        drawCell(snake.body[1].x, snake.body[1].y, 'o', COLOR_SNAKE_BODY);
    }
    
    drawCell(snake.body[0].x, snake.body[0].y, 'O', COLOR_SNAKE_HEAD);

    
    drawCell(food.position.x, food.position.y, '*', COLOR_FOOD);
}



void gameOver() {

    gotoxy(0, HEIGHT + 5);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR_TEXT);
    printf("Game Over! Final Score: %d    (Press any key to return to menu)   ", score);

   
    if (score > highScore) {
        highScore = score;
        strncpy(highScoreName, playerName, sizeof(highScoreName)-1);
        highScoreName[sizeof(highScoreName)-1] = '\0';
        saveHighScore();
    }

    _getch(); 
}



void beepEat() {

    Beep(1000, 60);
}

void beepGameOver() {

    Beep(400, 200);
    Beep(250, 200);
}