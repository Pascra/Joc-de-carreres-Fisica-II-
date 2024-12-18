#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"

class ModulePlayer : public Module
{
public:
    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

private:
    Texture2D car_texture;     // Textura del coche
    Vector2 car_position;      // Posición del coche
    float car_rotation;        // Rotación del coche
    float speed;               // Velocidad del coche
    float acceleration;        // Velocidad de rotación
};
