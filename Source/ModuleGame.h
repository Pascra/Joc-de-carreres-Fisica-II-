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



    Music introMusic;
    Music countdownMusic;
    Music playerswinMusic;
    Music aiwinsMusic;
    Music aiMusic;
 
    float player2WinMusicTimer = 0.0f; // Inicializa el temporizador
    float player1WinMusicTimer = 0.0f; // Inicializa el temporizador
    float AIWinMusicTimer = 0.0f; // Inicializa el temporizador

    bool is_touching_slowdown_p1 = false;
    bool is_touching_slowdown_p2 = false;

    void OnExitCollision(PhysBody* sensor, PhysBody* other);
    bool IsPointInRectangle(int x, int y, int rect_x, int rect_y, int width, int height);
private:

    // Textura para la pantalla de introducci�n
    Texture2D intro_texture;

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
    PhysBody* slowdown_zone = nullptr;

    void MoveAI(float deltaTime);

    float countdown_timer = 4.0f; // Inicia con 3 segundos

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
