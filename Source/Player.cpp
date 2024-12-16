#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"

// Constructor
ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), position{ 100, 100 }, speed(5.0f), rotation(0.0f)
{
}

// Destructor
ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
    LOG("Loading player");

    // Cargar la textura del coche
    car_texture = LoadTexture("Assets/pitstop_car_3_right.png");

    // Verificar si la textura se cargó correctamente
    if (car_texture == nullptr) {
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

    // Unload the car texture
    UnloadTexture(car_texture);

    return true;
}

// Update: mueve el coche y lo dibuja
update_status ModulePlayer::Update()
{
    // Movimiento del coche
    if (IsKeyDown(KEY_W)) // Mover hacia arriba
    {
        position.y -= speed;
    }
    if (IsKeyDown(KEY_S)) // Mover hacia abajo
    {
        position.y += speed;
    }
    if (IsKeyDown(KEY_A)) // Mover hacia izquierda
    {
        position.x -= speed;
        rotation = -90.0f; // Rotación hacia la izquierda
    }
    if (IsKeyDown(KEY_D)) // Mover hacia derecha
    {
        position.x += speed;
        rotation = 90.0f; // Rotación hacia la derecha
    }

    // Verificar las coordenadas de la posición
    LOG("Player Position: x = %f, y = %f", position.x, position.y);

    // Verificar que el coche está dentro de los límites de la pantalla
    position.x = Clamp(position.x, 0, screen_width - car_width);
    position.y = Clamp(position.y, 0, screen_height - car_height);

    // Dibujar el coche sin rotación para depuración
    DrawTexture(car_texture, position.x, position.y);

    return UPDATE_CONTINUE;
}
