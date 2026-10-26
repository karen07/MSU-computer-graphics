# MSU computer graphics

An interactive 3D OpenGL scene created as a computer graphics course project. It uses GLSL
shaders and supports runtime control of fog, depth of field, anti-aliasing, and
post-processing effects.

## Описание

Интерактивная трёхмерная сцена на OpenGL для учебного проекта по компьютерной графике.
Использует GLSL-шейдеры и позволяет во время работы управлять туманом, глубиной резкости,
сглаживанием и постобработкой.

В реализации используются GLFW, GLEW, GLM и PNG-текстуры. Можно перемещаться по сцене и
настраивать FXAA и другие графические эффекты во время работы.

В репозитории вместе хранятся код сцены, шейдеры, текстуры и управление. Это самостоятельная демонстрация, а не переиспользуемый графический движок.

![Scene](https://github.com/user-attachments/assets/fc2fa621-3351-4b72-b9af-4222462b1469)

## Требования

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

## Структура проекта

- `src/` - основной код сцены и render loop;
- `shader/` - vertex/fragment GLSL shaders;
- `texture/` - текстуры и normal maps;
- `mesh/` - геометрические примитивы;
- `glslprogram/` - вспомогательная обертка для GLSL programs.
