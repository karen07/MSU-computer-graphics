# MSU computer graphics

MSU computer graphics is an interactive 3D OpenGL scene built as a compact computer graphics course project.

The renderer uses GLSL shaders together with GLFW, GLEW, GLM and PNG textures. Runtime controls can switch fog, depth of field, FXAA and post-processing effects while moving through the scene.

The repository keeps the scene code, shaders, textures and controls together as a self-contained demonstration rather than a reusable rendering engine.

## Описание

MSU computer graphics - интерактивная трехмерная сцена на OpenGL, сделанная как компактный учебный проект по компьютерной графике.

Рендерер использует шейдеры GLSL вместе с GLFW, GLEW, GLM и текстурами PNG. Во время работы можно переключать туман, глубину резкости, FXAA и эффекты постобработки и перемещаться по сцене.

В репозитории вместе хранятся код сцены, шейдеры, текстуры и управление. Это самостоятельная демонстрация, а не переиспользуемый графический движок.

![Scene](https://github.com/user-attachments/assets/fc2fa621-3351-4b72-b9af-4222462b1469)

## Зависимости

Ubuntu:

```sh
sudo apt update && sudo apt install -y \
    build-essential \
    libglm-dev \
    liblodepng-dev \
    libglfw3-dev \
    libglew-dev \
    cmake
```

## Сборка

```sh
cmake --preset release
cmake --build --preset release
```

Исполняемый файл:

```text
build/release/computer-graphics
```

Шейдеры и текстуры открываются по путям относительно каталога `build/`, поэтому запускать программу удобнее из него:

```sh
cd build
./release/computer-graphics
```

## Управление

| Клавиша | Действие |
|:---:|---|
| WASD | Перемещение |
| P | Post effect |
| E | Depth of field |
| F | Fog |
| X | FXAA |
| +/- | Смена текстуры стены |

## Структура

- `src/` - основной код сцены и render loop;
- `shader/` - vertex/fragment GLSL shaders;
- `texture/` - текстуры и normal maps;
- `mesh/` - геометрические примитивы;
- `glslprogram/` - вспомогательная обертка для GLSL programs.
