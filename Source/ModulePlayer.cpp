#include "ModulePlayer.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"

// Constructor
ModulePlayer::ModulePlayer(Application* app, bool start_enabled)
    : Module(app, start_enabled), car_texture{ 0 }, car_position{ 400.0f, 300.0f }, car_rotation(0.0f), speed(200.0f), acceleration(100.0f)
{
}

// Destructor
ModulePlayer::~ModulePlayer() {}

// Load assets
bool ModulePlayer::Start()
{
    LOG("Loading player assets");

    car_texture = LoadTexture("Assets/coche.png");
    if (car_texture.id == 0)
    {
        LOG("ERROR: Failed to load car texture!");
        return false;
    }

    LOG("Car texture loaded successfully.");
    return true;
}

// Update: Lógica de movimiento y dibujo del coche
update_status ModulePlayer::Update()
{
    float delta_time = GetFrameTime();

    // Controles de movimiento
    if (IsKeyDown(KEY_W))
    {
        car_position.x += cos(car_rotation * DEG2RAD) * speed * delta_time;
        car_position.y += sin(car_rotation * DEG2RAD) * speed * delta_time;
    }
    if (IsKeyDown(KEY_S))
    {
        car_position.x -= cos(car_rotation * DEG2RAD) * speed * delta_time;
        car_position.y -= sin(car_rotation * DEG2RAD) * speed * delta_time;
    }
    if (IsKeyDown(KEY_A))
    {
        car_rotation -= acceleration * delta_time;
    }
    if (IsKeyDown(KEY_D))
    {
        car_rotation += acceleration * delta_time;
    }

    // Limitar el coche dentro de los límites de la pantalla
    car_position.x = Clamp(car_position.x, 0.0f, (float)(SCREEN_WIDTH - car_texture.width));
    car_position.y = Clamp(car_position.y, 0.0f, (float)(SCREEN_HEIGHT - car_texture.height));

    // Dibujar el coche
    DrawTexturePro(
        car_texture,
        Rectangle{ 0.0f, 0.0f, (float)car_texture.width, (float)car_texture.height },
        Rectangle{ car_position.x, car_position.y, (float)car_texture.width, (float)car_texture.height },
        Vector2{ (float)car_texture.width / 2.0f, (float)car_texture.height / 2.0f },
        car_rotation,
        WHITE
    );

    return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
    LOG("Unloading player assets");
    UnloadTexture(car_texture);
    return true;
}
