#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"
#include "ModulePhysics.h"

class ModulePlayer : public Module
{
public:
    ModulePlayer(Application* app, bool start_enabled = true);
    ~ModulePlayer();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

private:
    // Jugador 1
    Texture2D car_texture;     // Textura del coche del jugador 1
    Vector2 car_position;      // Posición del coche del jugador 1
    float car_rotation;        // Rotación del coche del jugador 1
    float speed;               // Velocidad actual del coche del jugador 1

    // Jugador 2
    Texture2D car_texture2;    // Textura del coche del jugador 2
    Vector2 player2_position;  // Posición del coche del jugador 2
    float player2_rotation;    // Rotación del coche del jugador 2
    float player2_speed;       // Velocidad actual del coche del jugador 2

    // Propiedades comunes
    float acceleration;        // Aceleración de ambos coches
    float max_speed;           // Velocidad máxima de ambos coches
    float handling;            // Manejo del coche (agilidad al girar)

    // Cuerpos físicos
    PhysBody* car_body;        // Cuerpo físico del jugador 1
    PhysBody* player2_body;    // Cuerpo físico del jugador 2

    bool debug;                // Modo depuración
};
