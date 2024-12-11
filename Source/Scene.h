// Scene.h
#ifndef SCENE_H
#define SCENE_H

#include "raylib.h"
#include <string>
#include <vector>

struct Tile {
    int id;
    Rectangle sourceRect;
    Vector2 position;
};

class Scene {
public:
    Scene();
    ~Scene();

    // Carga el mapa desde un archivo .tmx
    bool LoadMap(const std::string& mapPath);

    // Renderiza el mapa y la escena
    void Render();

    // Actualiza la lógica de la escena
    void Update();

    // Limpia los recursos
    void Unload();

private:
    Texture2D tilesetTexture; // Textura de tileset
    std::vector<Tile> tiles; // Tiles individuales renderizados
    bool mapLoaded;          // Indica si el mapa está cargado
};

#endif // SCENE_H
