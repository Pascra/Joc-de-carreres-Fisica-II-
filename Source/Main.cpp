// main.cpp
#include "Scene.h"
#include "raylib.h"
#include <iostream>

#define TITLE "Raylib TMX Viewer"

int main() {
    // Inicializa la ventana
    InitWindow(800, 600, TITLE);
    SetTargetFPS(60);

    Scene scene;

    if (!scene.LoadMap("Assets/CircuitoFinalTotal.tmx")) {
        std::cerr << "Failed to load map!" << std::endl;
        CloseWindow();
        return -1;
    }

    while (!WindowShouldClose()) {
        scene.Update(); // Actualiza la lógica de la escena

        BeginDrawing();
        ClearBackground(RAYWHITE);

        scene.Render(); // Renderiza el mapa

        EndDrawing();
    }

    scene.Unload(); // Limpia recursos
    CloseWindow();

    return 0;
}