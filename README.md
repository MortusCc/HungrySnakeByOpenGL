# 2D贪吃蛇游戏 - OpenGL实现
A 2D Snake Game Implemented with OpenGL and C++

## 项目介绍
本项目是基于 **C++** 和 **OpenGL** 图形库开发的经典2D贪吃蛇游戏，结合计算机图形学核心技术（如正交投影、像素级渲染、双缓冲机制）实现流畅的游戏体验。支持边界穿越、蛇身颜色渐变、动态难度调整等核心功能，代码结构清晰，环境配置步骤详尽，适合作为OpenGL入门实践项目。

## 核心功能
- 🎮 基础玩法：WASD方向控制，吃食物变长、得分累积
- 🎨 视觉效果：蛇身从尾到头平滑颜色渐变，提升视觉辨识度
- ⚡ 流畅交互：解决快速输入反向行进问题，暂停/继续无延迟响应
- 📈 动态难度：蛇身每增加5节，移动速度自动提升（最低50ms/帧）
- 🔄 边界规则：支持穿越边界（从一侧穿出后从对侧进入）
- 📊 状态管理：完整的运行/暂停/游戏结束状态切换，支持重启与退出

## 环境配置（Windows系统）
### 前置依赖
- 操作系统：Windows 10/11（64位）
- 开发工具：VSCode（个人使用，以下环境配置针对VScode）、Visual Studio（配置会更简单）
- 工具链：MSYS2 + MinGW-w64（提供C++编译器与OpenGL依赖库）

### 步骤1：安装MSYS2
1. 从[MSYS2官网](https://www.msys2.org/)下载安装包，默认安装路径为 `C:\msys64`（建议保持默认）
2. 安装完成后，启动 `MSYS2 MinGW x64` 终端（注意：必须选择x64版本）

### 步骤2：安装MinGW-w64工具链与OpenGL依赖库
在MSYS2终端中依次执行以下命令（更新包数据库并安装核心依赖）：
```bash
# 第一步：更新包数据库（需重复执行直到无更新）
pacman -Syu

# 第二步：安装C++编译器、OpenGL相关库
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-freeglut mingw-w64-x86_64-glew mingw-w64-x86_64-glm
```

### 步骤3：配置系统环境变量
1. 右键「此电脑」→「属性」→「高级系统设置」→「环境变量」
2. 在「系统变量」中找到 `Path`，点击「编辑」→「新建」，添加路径：
   ```
   C:\msys64\mingw64\bin
   ```
3. 验证环境：打开新的CMD终端，输入以下命令，若显示编译器版本则配置成功：
   ```bash
   g++ --version
   ```

### 步骤4：VSCode配置（核心文件）
在项目根目录下创建 `.vscode` 文件夹，添加以下3个配置文件（直接复制粘贴即可）：

#### 1. c_cpp_properties.json（头文件与编译器路径）
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/mingw64/include",
                "C:/msys64/mingw64/include/GL"
            ],
            "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
```

#### 2. tasks.json（编译任务配置）
```json
{
    "tasks": [
        {
            "type": "cppbuild",
            "label": "HungrySnake-Compile",
            "command": "C:/msys64/mingw64/bin/g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${workspaceFolder}/*.cpp",
                "-o",
                "${workspaceFolder}/HungrySnakeByOpenGL.exe",
                "-lfreeglut",
                "-lopengl32",
                "-lglu32",
                "-lglew32",
                "-lm"
            ],
            "options": {
                "cwd": "C:/msys64/mingw64/bin"
            },
            "problemMatcher": ["$gcc"],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ],
    "version": "2.0.0"
}
```

#### 3. launch.json（调试配置，可选）
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "HungrySnake-Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/HungrySnakeByOpenGL.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "HungrySnake-Compile"
        }
    ]
}
```

### 步骤5：环境验证
创建测试文件 `test_opengl.cpp`，验证OpenGL环境是否正常工作：
```cpp
#include <GL/freeglut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.0, 0.0); glVertex2f(-0.5, -0.5);
        glColor3f(0.0, 1.0, 0.0); glVertex2f(0.5, -0.5);
        glColor3f(0.0, 0.0, 1.0); glVertex2f(0.0, 0.5);
    glEnd();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 300);
    glutCreateWindow("OpenGL Test");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
```
- 按 `Ctrl+Shift+B` 编译，生成可执行文件
- 运行文件，若弹出窗口显示彩色三角形，则环境配置成功

## 编译与运行
### 方式1：VSCode内编译运行
1. 将 `main.cpp` 放入项目根目录
2. 按 `Ctrl+Shift+B` 执行编译任务（生成 `HungrySnakeByOpenGL.exe`）
3. 直接运行生成的可执行文件，或按 `F5` 启动调试模式

### 方式2：直接运行可执行文件
- 若已编译成功，可直接双击 `HungrySnakeByOpenGL.exe` 启动游戏
- 启动后会同时打开两个窗口：游戏画面窗口 + 日志输出窗口（显示操作日志与分数）

## 游戏操作说明
| 按键       | 功能描述                  |
|------------|---------------------------|
| W/A/S/D    | 控制蛇的上下左右移动      |
| 空格键     | 暂停/继续游戏（游戏中生效）|
| R键        | 游戏结束后重启游戏        |
| ESC键      | 退出游戏                  |

### 游戏规则
1. 蛇吃到红色食物后，长度+1，分数+10
2. 蛇身每增加5节，移动速度加快（最低速度50ms/帧，避免过快失控）
3. 蛇头碰撞自身时游戏结束
4. 蛇穿越边界后，从对侧边界重新进入（无边界阻挡）

## 项目结构
```
├── .vscode/                # VSCode配置文件夹
│   ├── c_cpp_properties.json  # 编译器与头文件配置
│   ├── tasks.json             # 编译任务配置
│   └── launch.json            # 调试配置（可选）
├── main.cpp                # 游戏核心代码（逻辑+渲染+交互）
├── HungrySnakeByOpenGL.exe # 编译生成的可执行文件（运行后生成）
├── LiCENSE                 # 许可证
└── README.md               # 项目说明文档
```

## 注意事项
1. 系统兼容性：目前仅支持Windows系统，Linux/macOS需修改环境配置与依赖库路径
2. 依赖库版本：建议使用MSYS2默认安装的库版本，避免版本冲突
3. 窗口大小：游戏窗口默认600x600像素，修改 `WINDOW_WIDTH`/`WINDOW_HEIGHT` 后需确保能被 `GRID_SIZE`（20）整除
4. 内存释放：游戏退出时会自动释放动态内存，无需手动处理
5. 常见问题：
   - 库链接失败：检查 `tasks.json` 中库名是否正确（如 `lopengl32` 无拼写错误）
   - 头文件找不到：检查 `c_cpp_properties.json` 中 `includePath` 路径是否与MSYS2安装路径一致
   - 运行闪退：大概率是环境变量未配置成功，重新执行步骤3并验证编译器版本

## 技术栈
- 编程语言：C++17
- 图形库：OpenGL（FreeGLUT + GLEW + GLM）
- 开发工具：VSCode + MSYS2 + MinGW-w64
- 核心技术：正交投影、像素级渲染、双缓冲动画、事件驱动架构

## 致谢
本项目基于《计算机图形学实用教程（第4版）》核心知识点开发，感谢教材对计算机图形学基础理论与实践的详细讲解。