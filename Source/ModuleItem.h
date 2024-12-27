#pragma once

#include "Module.h"
#include "raylib.h"
#include "ModulePhysics.h"
#include "Timer.h"

// Forward declaration
class ModulePlayer;

// Estructura para gestionar la animaci�n
struct Animation {
    Rectangle* frames;     // Array de frames de la animaci�n
    int frame_count;      // N�mero total de frames
    int current_frame;    // Frame actual
    float frame_speed;    // Velocidad de la animaci�n (segundos por frame)
    float accumulated_time; // Tiempo acumulado para el cambio de frame
};

class ModuleItem : public Module
{
public:
    ModuleItem(Application* app, bool start_enabled = true);
    ~ModuleItem();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;
    /*void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
    void EndOnCollision(PhysBody* bodyA, PhysBody* bodyB);*/

public:
    void RespawnItem();
    void UpdateAnimation();
    void InitializeAnimation();

    Texture2D item_texture;        // Item texture
    Vector2 item_position;         // Current position
    PhysBody* item_body;          // Physics body for collision
    bool is_visible;              // Item visibility state
    Timer respawn_timer;          // Timer for respawning
    bool needs_respawn;           // Flag to control respawn
    Timer boost_timer;            // Timer para controlar la duraci�n del boost
    float respawn_time;
    bool boost_active;            // Flag para saber si el boost est� activo
    // Animaci�n
    Animation nitro;              // Estructura de la animaci�n
};