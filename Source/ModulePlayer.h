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

    Vector2 GetCarPosition() const;        // Obtiene la posición del coche del jugador 1
    PhysBody* GetCarBody() const;          // Obtiene el cuerpo físico del coche del jugador 1
    PhysBody* GetPlayer2Body() const;      // Obtiene el cuerpo físico del coche del jugador 2
    void ApplySpeedBoost(int playerNum);
    void RestoreSpeedBoost(int playerNum);
    update_status PostUpdate() override;
    void ResetPositions();

    uint32 derrape_fx;

    void ApplySpeedModifier(int playerNum, float modifier);
    void RestoreSpeedModifier(int playerNum);



private:
    // Jugador 1
    Texture2D car_texture;     // Textura del coche del jugador 1
    Vector2 car_position;      // Posición del coche del jugador 1
    float car_rotation;        // Rotación del coche del jugador 1
    float speed;               // Velocidad actual del coche del jugador 1 
    bool is_drifting;         // Estado de derrape
    float drift_factor;       // Factor de derrape
    float drift_angle;        // Ángulo adicional durante el derrape
    float drift_recovery;     // Velocidad de recuperación del derrape
    float drift_speed_multiplier; // Multiplicador de velocidad durante el derrape
    float lateral_velocity;   // Velocidad lateral durante el derrape

    // Jugador 2
    Texture2D car_texture2;    // Textura del coche del jugador 2
    Vector2 player2_position;  // Posición del coche del jugador 2
    float player2_rotation;    // Rotación del coche del jugador 2
    float player2_speed;       // Velocidad actual del coche del jugador 2
    bool is_drifting_p2; // Estado de derrape 
    float drift_factor_p2; // Factor de derrape
    float drift_angle_p2; // Ángulo adicional durante el derrape
    float lateral_velocity_p2;      // Velocidad lateral durante el derrape

    // Propiedades comunes
    float acceleration;        // Aceleración de ambos coches
    float max_speed;           // Velocidad máxima de ambos coches
    float handling;            // Manejo del coche (agilidad al girar)
    float base_speed_boost = 1.0f;    // Velocidad normal
    float current_speed_boost = 1.0f;  // Velocidad actual del boost
    float speed_boost;
    float speed_boost_player1;
    float speed_boost_player2;

   

    // Cuerpos físicos
    PhysBody* car_body;        // Cuerpo físico del jugador 1
    PhysBody* player2_body;    // Cuerpo físico del jugador 2

    bool debug;                // Modo depuración

    
};
