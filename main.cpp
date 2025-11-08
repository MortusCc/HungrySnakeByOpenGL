#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>

// 窗口配置
const int WINDOW_WIDTH = 600;    // 窗口宽度
const int WINDOW_HEIGHT = 600;   // 窗口高度
const int GRID_SIZE = 20;        // 蛇身/食物尺寸（像素，需整除窗口尺寸）

// 游戏状态枚举
enum GameState {
    RUNNING,  // 运行中
    PAUSED,   // 暂停
    GAME_OVER // 游戏结束
};

// 移动方向枚举
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// 蛇身节点结构体
struct SnakeNode {
    glm::vec2 pos;  // 节点坐标（x,y）
    SnakeNode(glm::vec2 p) : pos(p) {}
};

// 食物结构体
struct Food {
    glm::vec2 pos;  // 食物坐标
    Food(glm::vec2 p) : pos(p) {}
};

// 全局变量（游戏核心数据）
std::vector<SnakeNode> snake;    // 蛇身节点列表
Food* food = nullptr;             // 食物对象
Direction dir = RIGHT;            // 初始移动方向
Direction nextDir = RIGHT;        // 下一帧要改变的方向
GameState gameState = RUNNING;    // 初始游戏状态
int score = 0;                    // 分数
int moveSpeed = 150;              // 初始移动速度（实际上是每次刷新位置的间隔时间：ms）
int minSpeed = 50;                // 最小移动速度（避免过快）

// 初始化 OpenGL 环境（2D 正交投影）
void initOpenGL() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // 背景色：深灰
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 正交投影：窗口左下角(0,0)，右上角(WINDOW_WIDTH,WINDOW_HEIGHT)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// 生成随机食物（避免与蛇身重叠）
void generateFood() {
    // 随机生成坐标（需为 GRID_SIZE 整数倍，避免错位）
    int maxX = (WINDOW_WIDTH - GRID_SIZE) / GRID_SIZE;
    int maxY = (WINDOW_HEIGHT - GRID_SIZE) / GRID_SIZE;
    int x = (rand() % maxX) * GRID_SIZE;
    int y = (rand() % maxY) * GRID_SIZE;

    // 检查是否与蛇身重叠，重叠则重新生成
    bool overlap = false;
    for (const auto& node : snake) {
        if (node.pos.x == x && node.pos.y == y) {
            overlap = true;
            break;
        }
    }
    if (overlap) {
        generateFood();
        return;
    }

    // 创建/更新食物
    if (food != nullptr) delete food;
    food = new Food(glm::vec2(x, y));
    printf("New Food! at (%d, %d)\n", x, y);
}

// 初始化游戏（蛇初始状态、食物生成）
void initGame() {
    // 清空蛇身
    snake.clear();
    // 初始蛇身：3个节点（窗口中间偏左）
    int startX = WINDOW_WIDTH / 2 - GRID_SIZE * 2;
    int startY = WINDOW_HEIGHT / 2;
    snake.emplace_back(glm::vec2(startX, startY));
    snake.emplace_back(glm::vec2(startX + GRID_SIZE, startY));
    snake.emplace_back(glm::vec2(startX + GRID_SIZE * 2, startY));

    // 初始方向、分数、速度
    dir = RIGHT;
    nextDir = RIGHT;
    score = 0;
    moveSpeed = 150;
    gameState = RUNNING;
    printf("InitGame, Start Score = 0, Start Speed = 150ms\n");

    // 生成初始食物
    generateFood();
}

// 绘制矩形（使用像素级绘制，基于扫描线填充思想）
void drawRect(float x, float y, float width, float height) {
    glBegin(GL_POINTS);
    for (int i = (int)x; i < (int)(x + width); i++) {
        for (int j = (int)y; j < (int)(y + height); j++) {
            glVertex2i(i, j);
        }
    }
    glEnd();
}

// 渲染蛇身（始终保持从尾到头的渐变效果）
void renderSnake() {
    size_t snakeLen = snake.size();
    
    // 基础颜色定义
    const float baseRed = 0.2f;     // 基础红色分量
    const float baseGreen = 0.9f;   // 基础绿色分量
    const float baseBlue = 0.2f;    // 基础蓝色分量
    
    // 头部目标颜色（橙色）
    const float headRed = 0.9f;
    const float headGreen = 0.5f;
    const float headBlue = 0.2f;
    
    // 根据蛇的长度计算整体颜色深度因子
    // 开局时因子为0，颜色为鲜绿色；随着长度增加，因子逐渐增大到1
    float depthFactor = glm::clamp((float)(snakeLen - 3) / 30.0f, 0.0f, 1.0f);
    
    // 绘制除了头部以外的蛇身部分
    for (size_t i = 1; i < snakeLen; ++i) {
        // 计算当前节点在蛇身上的位置比例（0=尾部，1=头部方向）
        float positionFactor = 1.0f - (float)i / (snakeLen - 1);
        
        // 应用整体深度因子和位置因子
        float finalFactor = positionFactor * depthFactor;
        
        // 根据因子计算最终颜色
        float red = baseRed + finalFactor * (headRed - baseRed);
        float green = baseGreen + finalFactor * (headGreen - baseGreen);
        float blue = baseBlue + finalFactor * (headBlue - baseBlue);
        
        glColor3f(red, green, blue);
        drawRect(snake[i].pos.x, snake[i].pos.y, GRID_SIZE, GRID_SIZE);
    }
    
    // 绘制蛇头（稍微亮一点）
    float headFinalRed = baseRed + depthFactor * (headRed - baseRed);
    float headFinalGreen = baseGreen + depthFactor * (headGreen - baseGreen);
    float headFinalBlue = baseBlue + depthFactor * (headBlue - baseBlue);
    
    glColor3f(headFinalRed + 0.1f, headFinalGreen + 0.1f, headFinalBlue + 0.1f);
    drawRect(snake.front().pos.x, snake.front().pos.y, GRID_SIZE, GRID_SIZE);
}

// 渲染食物（红色）
void renderFood() {
    if (food == nullptr) return;
    glColor3f(0.9f, 0.2f, 0.2f);  // 食物颜色：红色
    drawRect(food->pos.x, food->pos.y, GRID_SIZE, GRID_SIZE);
}

// 渲染文本（分数、游戏状态提示）
void renderText(const char* text, int x, int y, void* font = GLUT_BITMAP_9_BY_15) {
    glRasterPos2i(x, y);          // 文本位置
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);  // 绘制字符
    }
}

// 渲染所有元素（OpenGL 核心渲染函数）
void display() {
    glClear(GL_COLOR_BUFFER_BIT);  // 清空颜色缓冲
    glLoadIdentity();

    // 1. 渲染蛇身和食物
    renderSnake();
    renderFood();

    // 2. 渲染分数（窗口顶部居中）
    char scoreText[64];
    sprintf(scoreText, "Score: %d | Speed Level: %d", score, (150 - moveSpeed) / 10 + 1);
    glColor3f(0.8f, 0.8f, 0.8f);  // 文本颜色：浅灰
    renderText(scoreText, WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT - 20);

    // 3. 渲染游戏状态提示
    if (gameState == PAUSED) {
        renderText("Pause! Press Space to Continue", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2);
    } else if (gameState == GAME_OVER) {
        renderText("Game Over! Press R to Restart | ESC to Quit", WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2);
    }

    glutSwapBuffers();  // 交换双缓冲（避免闪烁）
}

// 键盘控制回调（方向、暂停、重启、退出）
void keyboard(unsigned char key, int x, int y) {
    Direction proposedDir = dir;
    bool dirChanged = false;

    switch (key) {
        // WASD控制方向
        case 'W':
        case 'w': // 上
            if (gameState == RUNNING && dir != DOWN && dir != UP) {
                proposedDir = UP;
                dirChanged = true;
                printf("[UP], Up!\n");
            }
            break;
        case 'S':
        case 's': // 下
            if (gameState == RUNNING && dir != UP && dir != DOWN) {
                proposedDir = DOWN;
                dirChanged = true;
                printf("[DOWN], Down!\n");
            }
            break;
        case 'A':
        case 'a': // 左
            if (gameState == RUNNING && dir != RIGHT && dir != LEFT) {
                proposedDir = LEFT;
                dirChanged = true;
                printf("[LEFT], Left!\n");
            }
            break;
        case 'D':
        case 'd': // 右
            if (gameState == RUNNING && dir != LEFT && dir != RIGHT) {
                proposedDir = RIGHT;
                dirChanged = true;
                printf("[RIGHT], Right!\n");
            }
            break;
        
        // 空格键：暂停/继续
        case ' ':
            if (gameState != GAME_OVER) {
                gameState = (gameState == RUNNING) ? PAUSED : RUNNING;
                printf("[SPACE], %s Now!\n", gameState == PAUSED ? "Stop" : "Run");
                glutPostRedisplay(); // 强制重绘屏幕以立即显示暂停界面
            }
            break;
        
        // R 键：游戏重启（仅游戏结束时生效）
        case 'R':
        case 'r':
            if (gameState == GAME_OVER) {
                printf("[R], Game Restart!\n");
                initGame();
            }
            break;
        
        // ESC 键：退出游戏
        case 27:
            printf("[ESC], Game Quit!\n");
            delete food;
            exit(0);
            break;
    }

    if (dirChanged) {
        nextDir = proposedDir;
    }
}

// 碰撞检测（仅检测自身碰撞，移除边界碰撞）
bool checkCollision() {
    const auto& head = snake.front();

    // 自身碰撞（蛇头与蛇身节点重叠，从第5个节点开始检查避免误判）
    for (size_t i = 4; i < snake.size(); i++) {
        if (head.pos.x == snake[i].pos.x && head.pos.y == snake[i].pos.y) {
            printf("Game Over! Crush, Current Score: %d\n", score);
            return true;
        }
    }
    return false;
}

// 游戏逻辑更新（定时调用：移动、吃食物、碰撞检测）
void update(int value) {
    if (gameState != RUNNING) {
        glutPostRedisplay(); // 即使在暂停状态下也要重绘屏幕
        glutTimerFunc(moveSpeed, update, 0);  // 暂停时仍保持定时器
        return;
    }

    // 在每一帧开始时应用方向改变
    dir = nextDir;

    // 1. 计算新蛇头坐标
    glm::vec2 newHead = snake.front().pos;
    switch (dir) {
        case UP:    newHead.y += GRID_SIZE; break;
        case DOWN:  newHead.y -= GRID_SIZE; break;
        case LEFT:  newHead.x -= GRID_SIZE; break;
        case RIGHT: newHead.x += GRID_SIZE; break;
    }

    // 添加边界穿越逻辑
    // 如果蛇头超出右边界，则从左边界进入
    if (newHead.x >= WINDOW_WIDTH) {
        newHead.x = 0;
    }
    // 如果蛇头超出左边界，则从右边界进入
    else if (newHead.x < 0) {
        newHead.x = WINDOW_WIDTH - GRID_SIZE;
    }
    // 如果蛇头超出上边界，则从下边界进入
    if (newHead.y >= WINDOW_HEIGHT) {
        newHead.y = 0;
    }
    // 如果蛇头超出下边界，则从上边界进入
    else if (newHead.y < 0) {
        newHead.y = WINDOW_HEIGHT - GRID_SIZE;
    }

    // 2. 检查是否吃到食物
    bool ateFood = false;
    if (food != nullptr && newHead.x == food->pos.x && newHead.y == food->pos.y) {
        ateFood = true;
        score += 10;  // 吃到食物加10分
        printf("Food! Score + 10, Current Score: %d\n", score);
        generateFood();  // 重新生成食物
        
        // 蛇长度每增加5节，速度加快10ms（不低于最小速度）
        if (snake.size() % 5 == 0 && moveSpeed > minSpeed) {
            moveSpeed -= 10;
            printf("Lenth Up, Speed Up! Current Speed Level: %d\n", (150 - moveSpeed) / 10 + 1);
        }
    }

    // 3. 更新蛇身：添加新头部，未吃食物则删除尾部
    snake.insert(snake.begin(), SnakeNode(newHead));
    if (!ateFood) {
        snake.pop_back();
    }

    // 4. 碰撞检测：触发游戏结束
    if (checkCollision()) {
        gameState = GAME_OVER;
    }

    // 5. 重新注册定时器（循环更新）
    glutPostRedisplay();  // 触发重渲染
    glutTimerFunc(moveSpeed, update, 0);
}

// 窗口大小改变时调整投影
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

// 主函数（程序入口）
int main(int argc, char** argv) {
    printf("===== Hungry Snake Log =====\n");
    // 1. 初始化 freeglut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  // 双缓冲+RGB颜色
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(600, 100);  // 窗口初始位置
    glutCreateWindow("2D Snake - OpenGL");

    // 2. 初始化 glew（需在 glutCreateWindow 后调用）
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW initialization failed!\n");
        return 1;
    }

    // 3. 初始化游戏和 OpenGL
    srand((unsigned int)time(nullptr));  // 随机种子（食物生成用）
    initOpenGL();
    initGame();

    // 4. 注册 OpenGL 回调函数
    glutDisplayFunc(display);    // 渲染回调
    glutReshapeFunc(reshape);    // 窗口大小改变回调
    glutKeyboardFunc(keyboard);  // 键盘控制回调
    glutTimerFunc(moveSpeed, update, 0);  // 定时更新回调

    // 5. 启动 OpenGL 主循环
    glutMainLoop();

    // 6. 释放资源（理论上 glutMainLoop 不会返回，此处为规范）
    delete food;
    return 0;
}