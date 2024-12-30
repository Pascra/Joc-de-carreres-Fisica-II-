#pragma once
#include "Module.h"
#include "raylib.h"
#include "Utils.h"
#include "ModulePhysics.h"

class ModuleItem : public Module
{
public:
    ModuleItem(Application* app, bool start_enabled = true);
    ~ModuleItem();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;
    void OnCollision(PhysBody* body1, PhysBody* body2) override;

private:
    Texture2D item_texture;        // Textura del item
    Vector2 item_position;         // Posición del item
    PhysBody* item_sensor;         // Sensor físico del item
    bool item_active;              // Estado del item (visible/invisible)
    float respawn_timer;           // Temporizador para reaparecer
    float respawn_time;            // Tiempo que tarda en reaparecer

    // Variables para la animación
    Rectangle frame_rec;           // Rectangle para el frame actual
    int current_frame;             // Frame actual de la animación
    int frames_counter;            // Contador para la animación
    int frames_speed;              // Velocidad de la animación

    // Variables para el boost
    float boost_duration;          // Duración del boost
    float boost_timer_p1;          // Temporizador del boost para player 1
    float boost_timer_p2;          // Temporizador del boost para player 2
    bool is_boosted_p1;           // Estado del boost para player 1
    bool is_boosted_p2;           // Estado del boost para player 2

    void UpdateBoostTimers();      // Actualiza los temporizadores de boost
    void ApplyBoostToPlayer(int playerNum); // Aplica el boost al jugador
};