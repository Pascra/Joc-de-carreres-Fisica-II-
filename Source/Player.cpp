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
    car_texture = LoadTexture("Assets/pitstop_car_3_right.png"); // Asegúrate de que el archivo esté en la ruta correcta

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

    // Actualizar la posición del coche en función de las teclas presionadas
    // Dibujar el coche (con rotación)
    DrawTextureEx(car_texture, position, rotation, 1.0f, WHITE);

    return UPDATE_CONTINUE;
}
