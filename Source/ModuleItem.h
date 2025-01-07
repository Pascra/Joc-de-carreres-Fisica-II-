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
    // Item 1
    Texture2D item_texture;
    Vector2 item_position;
    PhysBody* item_sensor;
    bool item_active;
    float respawn_timer;
    float respawn_time;

    // Solo añadir estas variables para el segundo item
    Vector2 item_position2;
    PhysBody* item_sensor2;
    bool item_active2;
    float respawn_timer2;

    // El resto igual que estaba
    Rectangle frame_rec;
    int current_frame;
    int frames_counter;
    int frames_speed;

    float boost_duration;
    float boost_timer_p1;
    float boost_timer_p2;
    bool is_boosted_p1;
    bool is_boosted_p2;

    void UpdateBoostTimers();
    void ApplyBoostToPlayer(int playerNum);
};