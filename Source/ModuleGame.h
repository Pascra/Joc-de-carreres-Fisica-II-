#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"
#include "ModulePhysics.h"
#include <vector>

class ModuleGame : public Module
{
public:
    ModuleGame(Application* app, bool start_enabled = true);
    ~ModuleGame();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

    void OnCollision(PhysBody* sensor, PhysBody* other);

private:
    Texture2D map_texture; // Textura del mapa
    std::vector<PhysBody*> checkpoint_sensors; // Lista de sensores
    PhysBody* finish_line; // Sensor de la línea de meta
    int current_checkpoint = 0; // Progreso en los checkpoints
    int laps = 0; // Contador de vueltas
};
struct Checkpoint
{
    int x;      // Coordenada X del centro
    int y;      // Coordenada Y del centro
    int width;  // Ancho del sensor
    int height; // Altura del sensor
};