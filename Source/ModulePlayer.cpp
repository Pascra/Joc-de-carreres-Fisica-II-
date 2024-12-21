// ModulePlayer.cpp
#include "ModulePlayer.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"

// Constructor
ModulePlayer::ModulePlayer(Application* app, bool start_enabled)
    : Module(app, start_enabled), car_texture{ 0 }, car_position{ 400.0f, 300.0f }, car_rotation(0.0f), speed(0.0f), acceleration(200.0f), max_speed(400.0f), handling(100.0f)
{
}

// Destructor
ModulePlayer::~ModulePlayer() {}

// Load assets
bool ModulePlayer::Start()
{
    LOG("Loading player assets");

    car_texture = LoadTexture("Assets/cars/pitstop_car_1.png");
    if (car_texture.id == 0)
    {
        LOG("ERROR: Failed to load car texture!");
        return false;
    }

    // Escala del sprite
    float scale = 0.2f;

    // Centrar el coche en el mapa considerando la escala
    car_position.x = (754.0f / 2.0f) - ((car_texture.width * scale) / 2.0f);
    car_position.y = (426.0f / 2.0f) - ((car_texture.height * scale) / 2.0f);

    car_rotation = 0.0f; // Rotación inicial
    speed = 0.0f;        // Velocidad inicial

    LOG("Player initialized at position: (%f, %f)", car_position.x, car_position.y);

    return true;
}




// Update: Lógica de movimiento y dibujo del coche
update_status ModulePlayer::Update()
{
    float delta_time = GetFrameTime();

    // Controles de movimiento
    if (IsKeyDown(KEY_W)) // Acelerar hacia adelante
    {
        speed += acceleration * delta_time;
        if (speed > max_speed)
            speed = max_speed;
    }
    else if (IsKeyDown(KEY_S)) // Frenar/marcha atrás
    {
        speed -= acceleration * delta_time;
        if (speed < -max_speed / 2) // Velocidad máxima en reversa es menor
            speed = -max_speed / 2;
    }
    else // Desaceleración natural
    {
        speed *= 0.95f; // Reducir velocidad gradualmente
        if (fabs(speed) < 10.0f)
            speed = 0.0f;
    }

    if (IsKeyDown(KEY_A)) // Girar a la izquierda
    {
        car_rotation -= handling * delta_time;
    }
    if (IsKeyDown(KEY_D)) // Girar a la derecha
    {
        car_rotation += handling * delta_time;
    }

    // Ajustar el movimiento para que coincida con la orientación del sprite
    float adjusted_rotation = car_rotation - 90.0f;

    // Calcular la posición del coche sin límites
    car_position.x -= cos(adjusted_rotation * DEG2RAD) * speed * delta_time;
    car_position.y -= sin(adjusted_rotation * DEG2RAD) * speed * delta_time;

    // Escalar el sprite del coche
    float scale = 0.2f; // Reducir tamaño del coche al 20% de su tamaño original

    // Dibujar el coche con el sprite
    DrawTexturePro(
        car_texture,
        Rectangle{ 0.0f, 0.0f, (float)car_texture.width, (float)car_texture.height }, // Toda la textura
        Rectangle{ car_position.x, car_position.y, (float)car_texture.width * scale, (float)car_texture.height * scale }, // Escalado
        Vector2{ (float)car_texture.width * scale / 2.0f, (float)car_texture.height * scale / 2.0f }, // Centro ajustado
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
