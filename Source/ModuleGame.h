#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"
#include "ModulePhysics.h"
#include <vector>

// Estructura para los checkpoints
struct Checkpoint
{
    int x;      // Coordenada X del centro
    int y;      // Coordenada Y del centro
    int width;  // Ancho del sensor
    int height; // Altura del sensor
};

class ModuleGame : public Module
{
public:
    ModuleGame(Application* app, bool start_enabled = true);
    ~ModuleGame();

    bool Start() override;
    update_status Update() override;
    void OnCollision(PhysBody* sensor, PhysBody* other) override;
    bool CleanUp() override;

private:
    // Mapa
    Texture2D map_texture;

    // Checkpoints
    std::vector<Checkpoint> checkpoint_definitions; // Lista de checkpoints
    std::vector<PhysBody*> checkpoint_sensors;      // Sensores f�sicos de los checkpoints

    // L�nea de meta
    PhysBody* finish_line;

    // Progreso de los jugadores
    int current_checkpoint_player1;
    int laps_player1;
    int current_checkpoint_player2;
    int laps_player2;

    // Progreso de la IA
    Texture2D ai_texture;       // Textura del coche de la IA
    Vector2 ai_position;        // Posici�n del coche de la IA
    float ai_rotation;          // Rotaci�n del coche de la IA
    float ai_speed;             // Velocidad actual del coche de la IA
    PhysBody* ai_body;          // Cuerpo f�sico del coche de la IA
    int current_checkpoint_ai;  // �ndice del checkpoint actual que la IA persigue
    int laps_ai;

    void MoveAI(float deltaTime);

};
