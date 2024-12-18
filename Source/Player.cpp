#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "Player.h"

// Constructor
ModulePlayer::ModulePlayer(Application* app, bool start_enabled)
    : Module(app, start_enabled), position{ 100, 100 }, speed(5.0f), rotation(0.0f)
{
}

// Destructor
ModulePlayer::~ModulePlayer() {}

// Load assets
bool ModulePlayer::Start()
{
    LOG("Loading player");

    car_texture = LoadTexture("Assets/pitstop_car_3_right.png");
    if (car_texture.id == 0) {
        LOG("Failed to load car texture!");
        return false;
    }
    else {
        LOG("Car texture loaded successfully.");
    }

    return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
    LOG("Unloading player");
    UnloadTexture(car_texture);
    return true;
}

// Update: mueve el coche y lo dibuja
update_status ModulePlayer::Update()
{
    // Movimiento del coche
    if (IsKeyDown(KEY_W)) position.y -= speed;
    if (IsKeyDown(KEY_S)) position.y += speed;
    if (IsKeyDown(KEY_A)) {
        position.x -= speed;
        rotation = -90.0f;
    }
    if (IsKeyDown(KEY_D)) {
        position.x += speed;
        rotation = 90.0f;
    }

    // Verificar límites
    position.x = Clamp(position.x, 0.0f, screen_width - car_texture.width);
    position.y = Clamp(position.y, 0.0f, screen_height - car_texture.height);

    // Dibujar el coche con rotación
    DrawTextureEx(car_texture, position, rotation, 1.0f, WHITE);

    return UPDATE_CONTINUE;
}
