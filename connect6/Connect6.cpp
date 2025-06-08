#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>

// 游戏常量定义
#define BOARD_SIZE 19      // 棋盘大小 19x19
#define GRID_SIZE 30       // 每个格子的大小（像素）
#define MARGIN 30          // 棋盘边距
#define CHESS_RADIUS 13    // 棋子半径

// 棋子类型
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// 游戏状态
#define GAME_PLAYING 0
#define GAME_WIN 1
#define GAME_DRAW 2

// 全局变量
int chessBoard[BOARD_SIZE][BOARD_SIZE] = {0};  // 棋盘状态，0表示空，1表示黑棋，2表示白棋
int currentPlayer = BLACK;                     // 当前玩家
int gameState = GAME_PLAYING;                  // 游戏状态
int moveCount = 0;                            // 移动次数
int placedCount = 0;                          // 当前回合已放置的棋子数

// 函数声明
void initGame();                              // 初始化游戏
void drawBoard();                             // 绘制棋盘
void drawChess();                             // 绘制棋子
void drawGameInfo();                          // 绘制游戏信息
bool placeChess(int row, int col);            // 放置棋子
bool checkWin(int row, int col);              // 检查是否获胜
bool isBoardFull();                           // 检查棋盘是否已满
void switchPlayer();                          // 切换玩家

// 主函数
int main() {
    initGame();
    
    ExMessage msg;
    bool quit = false;
    
    while (!quit) {
        // 绘制游戏界面
        BeginBatchDraw();
        drawBoard();
        drawChess();
        drawGameInfo();
        EndBatchDraw();
        
        // 处理用户输入
        if (peekmessage(&msg, EM_MOUSE)) {
            if (msg.message == WM_LBUTTONDOWN && gameState == GAME_PLAYING) {
                // 计算点击的行列
                int col = (msg.x - MARGIN + GRID_SIZE / 2) / GRID_SIZE;
                int row = (msg.y - MARGIN + GRID_SIZE / 2) / GRID_SIZE;
                
                // 确保点击在棋盘范围内
                if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
                    // 尝试放置棋子
                    if (placeChess(row, col)) {
                        // 检查是否获胜
                        if (checkWin(row, col)) {
                            gameState = GAME_WIN;
                        }
                        // 检查是否平局
                        else if (isBoardFull()) {
                            gameState = GAME_DRAW;
                        }
                        
                        // 如果已经放置了足够的棋子，切换玩家
                        if (placedCount == (moveCount == 0 ? 1 : 2)) {
                            switchPlayer();
                            placedCount = 0;
                            moveCount++;
                        }
                    }
                }
            }
            else if (msg.message == WM_RBUTTONDOWN) {
                // 右键点击重新开始游戏
                if (gameState != GAME_PLAYING) {
                    initGame();
                }
            }
            else if (msg.message == WM_KEYDOWN) {
                if (msg.vkcode == VK_ESCAPE) {
                    quit = true;
                }
            }
        }
        
        Sleep(10); // 降低CPU使用率
    }
    
    closegraph();
    return 0;
}

// 初始化游戏
void initGame() {
    // 初始化图形窗口
    initgraph(BOARD_SIZE * GRID_SIZE + MARGIN * 2, BOARD_SIZE * GRID_SIZE + MARGIN * 2);
    setbkcolor(RGB(240, 220, 180)); // 设置背景色为浅黄色
    cleardevice();
    
    // 初始化棋盘
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            chessBoard[i][j] = EMPTY;
        }
    }
    
    // 初始化游戏状态
    currentPlayer = BLACK;
    gameState = GAME_PLAYING;
    moveCount = 0;
    placedCount = 0;
}

// 绘制棋盘
void drawBoard() {
    cleardevice(); // 清屏
    
    // 设置线条颜色和样式
    setlinecolor(BLACK);
    setlinestyle(PS_SOLID, 1);
    
    // 绘制棋盘格子
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 绘制水平线
        line(MARGIN, MARGIN + i * GRID_SIZE, 
             MARGIN + (BOARD_SIZE - 1) * GRID_SIZE, MARGIN + i * GRID_SIZE);
        
        // 绘制垂直线
        line(MARGIN + i * GRID_SIZE, MARGIN, 
             MARGIN + i * GRID_SIZE, MARGIN + (BOARD_SIZE - 1) * GRID_SIZE);
    }
    
    // 绘制天元和星位
    setfillcolor(BLACK);
    // 天元（中心点）
    fillcircle(MARGIN + (BOARD_SIZE - 1) / 2 * GRID_SIZE, 
              MARGIN + (BOARD_SIZE - 1) / 2 * GRID_SIZE, 4);
    
    // 星位（四角和边中点）
    int starPoints[6][2] = {
        {3, 3}, {3, BOARD_SIZE - 4}, 
        {BOARD_SIZE - 4, 3}, {BOARD_SIZE - 4, BOARD_SIZE - 4},
        {3, (BOARD_SIZE - 1) / 2}, {BOARD_SIZE - 4, (BOARD_SIZE - 1) / 2}
    };
    
    for (int i = 0; i < 6; i++) {
        fillcircle(MARGIN + starPoints[i][0] * GRID_SIZE, 
                  MARGIN + starPoints[i][1] * GRID_SIZE, 4);
    }
}

// 绘制棋子
void drawChess() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (chessBoard[i][j] != EMPTY) {
                // 设置棋子颜色
                if (chessBoard[i][j] == BLACK) {
                    setfillcolor(BLACK);
                } else {
                    setfillcolor(WHITE);
                }
                
                // 绘制棋子
                fillcircle(MARGIN + j * GRID_SIZE, MARGIN + i * GRID_SIZE, CHESS_RADIUS);
                
                // 为白棋添加黑色边框
                if (chessBoard[i][j] == WHITE) {
                    setlinecolor(BLACK);
                    circle(MARGIN + j * GRID_SIZE, MARGIN + i * GRID_SIZE, CHESS_RADIUS);
                }
            }
        }
    }
}

// 绘制游戏信息
void drawGameInfo() {
    // 设置文本样式
    settextcolor(BLACK);
    settextstyle(20, 0, _T("宋体"));
    
    // 显示当前玩家
    TCHAR info[50];
    if (gameState == GAME_PLAYING) {
        if (currentPlayer == BLACK) {
            _stprintf(info, _T("当前玩家：黑棋 (还需放置 %d 颗棋子)"), (moveCount == 0 ? 1 : 2) - placedCount);
        } else {
            _stprintf(info, _T("当前玩家：白棋 (还需放置 %d 颗棋子)"), 2 - placedCount);
        }
    } else if (gameState == GAME_WIN) {
        if (currentPlayer == BLACK) {
            _stprintf(info, _T("白棋获胜！右键点击重新开始"));
        } else {
            _stprintf(info, _T("黑棋获胜！右键点击重新开始"));
        }
    } else { // GAME_DRAW
        _stprintf(info, _T("游戏平局！右键点击重新开始"));
    }
    
    outtextxy(MARGIN, 10, info);
}

// 放置棋子
bool placeChess(int row, int col) {
    // 检查位置是否为空
    if (chessBoard[row][col] != EMPTY) {
        return false;
    }
    
    // 放置棋子
    chessBoard[row][col] = currentPlayer;
    placedCount++;
    
    return true;
}

// 检查是否获胜
bool checkWin(int row, int col) {
    // 获取当前棋子类型
    int chessType = chessBoard[row][col];
    
    // 定义8个方向：水平、垂直、两个对角线
    int directions[8][2] = {
        {0, 1}, {1, 0}, {1, 1}, {1, -1},
        {0, -1}, {-1, 0}, {-1, -1}, {-1, 1}
    };
    
    // 检查每个方向
    for (int d = 0; d < 4; d++) {
        int count = 1; // 当前位置已经有一个棋子
        
        // 正向检查
        for (int i = 1; i <= 5; i++) {
            int newRow = row + directions[d][0] * i;
            int newCol = col + directions[d][1] * i;
            
            // 检查边界
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE) {
                break;
            }
            
            // 检查棋子类型
            if (chessBoard[newRow][newCol] == chessType) {
                count++;
            } else {
                break;
            }
        }
        
        // 反向检查
        for (int i = 1; i <= 5; i++) {
            int newRow = row + directions[d + 4][0] * i;
            int newCol = col + directions[d + 4][1] * i;
            
            // 检查边界
            if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE) {
                break;
            }
            
            // 检查棋子类型
            if (chessBoard[newRow][newCol] == chessType) {
                count++;
            } else {
                break;
            }
        }
        
        // 如果连续6个或以上相同棋子，则获胜
        if (count >= 6) {
            return true;
        }
    }
    
    return false;
}

// 检查棋盘是否已满
bool isBoardFull() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (chessBoard[i][j] == EMPTY) {
                return false;
            }
        }
    }
    return true;
}

// 切换玩家
void switchPlayer() {
    currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
}