#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"
#include "ModulePhysics.h"
#include <vector>
#include "p2Point.h"

class PhysicEntity;

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
    void DrawTime();
   
    bool IsPlayer1Winner() const { return player1_won; }
    bool IsGameOver() const { return player1_won; }

    std::vector<PhysicEntity*> entities;

    Texture2D default;

    void DrawLaps();



private:

    // Textura para la pantalla de introducción
    Texture2D intro_texture;

    // Mapa
    Texture2D map_texture;

    // Checkpoints
    std::vector<Checkpoint> checkpoint_definitions; // Lista de checkpoints
    std::vector<PhysBody*> checkpoint_sensors;      // Sensores físicos de los checkpoints

    // Línea de meta
    PhysBody* finish_line;

    // Progreso de los jugadores
    int current_checkpoint_player1;
    int laps_player1;
    int current_checkpoint_player2;
    int laps_player2;

    // Progreso de la IA
    Texture2D ai_texture;       // Textura del coche de la IA
    Vector2 ai_position;        // Posición del coche de la IA
    float ai_rotation;          // Rotación del coche de la IA
    float ai_speed;             // Velocidad actual del coche de la IA
    PhysBody* ai_body;          // Cuerpo físico del coche de la IA
    int current_checkpoint_ai;  // Índice del checkpoint actual que la IA persigue
    int laps_ai;

    void MoveAI(float deltaTime);


    //Time
    float time;
    float time_lap1;
    float time_lap2;
    float time_lap3;

    Texture2D player1_win_texture; // Textura para el mensaje de victoria del jugador 1
    bool player1_won = false;      // Bandera para indicar si el jugador 1 ha ganado
    Texture2D player2_win_texture; // Textura para el mensaje de victoria del jugador 1
    bool player2_won = false;
    Texture2D Ai_win_texture; // Textura para el mensaje de victoria del jugador 1
    bool Ai_won = false;
};
