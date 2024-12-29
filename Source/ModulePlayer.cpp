#include "ModulePlayer.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"
#include "ModuleGame.h"

// Constructor
ModulePlayer::ModulePlayer(Application* app, bool start_enabled)
    : Module(app, start_enabled), car_texture{ 0 }, car_texture2{ 0 },
    car_position{ 400.0f, 300.0f }, player2_position{ 300.0f, 200.0f },
    car_rotation(0.0f), player2_rotation(0.0f),
    speed(-10.0f), player2_speed(0.0f),
    speed_boost(1.0f), base_speed_boost(1.0f),
    acceleration(125.0f), max_speed(300.0f), handling(200.0f), car_body(nullptr), player2_body(nullptr)
{
}

// Destructor
ModulePlayer::~ModulePlayer() {}

// Load assets
bool ModulePlayer::Start()
{
    debug = false;
    LOG("Loading player assets");

    // Cargar textura del jugador 1
    car_texture = LoadTexture("Assets/cars/pitstop_car_1.png");
    if (car_texture.id == 0)
    {
        LOG("ERROR: Failed to load car texture for Player 1!");
        return false;
    }

    // Cargar textura del jugador 2
    car_texture2 = LoadTexture("Assets/cars/pitstop_car_2.png");
    if (car_texture2.id == 0)
    {
        LOG("ERROR: Failed to load car texture for Player 2!");
        return false;
    }

    // Escala del sprite
    float scale = 0.2f;

    // Inicializar jugador 1
    car_position.x = (1920.0f / 2.0f) - ((car_texture.width * scale) / 2.0f);
    car_position.y = (1144.0f / 2.0f) - ((car_texture.height * scale) / 2.0f);
    car_body = App->physics->CreateRectangle(
        car_position.x,
        car_position.y,
        car_texture.width * scale * 0.2f,  // Hacer la hitbox un poco más pequeña que el sprite
        car_texture.height * scale * 0.2f
    );
    car_body->ctype = CollisionType::PLAYER1;
    car_rotation = 180.0f;
    speed = 0.0f;

    // Inicializar jugador 2
    player2_position.x = 862.0f;
    player2_position.y = 484.0f;
    player2_body = App->physics->CreateRectangle(
        player2_position.x,
        player2_position.y,
        car_texture2.width * scale * 0.2f,
        car_texture2.height * scale * 0.2f
    );
    player2_body->ctype = CollisionType::PLAYER2;
    player2_rotation = 180.0f;
    player2_speed = 0.0f;

    LOG("Players initialized.");

    return true;
}

// Update: Lógica de movimiento y dibujo del coche
update_status ModulePlayer::Update()
{
    float delta_time = GetFrameTime();

    // Para el jugador 1
    if (IsKeyDown(KEY_W)) // Acelerar hacia adelante
    {
        speed += acceleration * delta_time;
        if (speed > max_speed)
            speed = max_speed;
        speed *= speed_boost;
    }
    else if (IsKeyDown(KEY_S)) // Frenar/marcha atrás
    {
        speed -= acceleration * delta_time;
        if (speed < -max_speed / 2) // Velocidad máxima en reversa es menor
            speed = -max_speed / 2;
        speed *= speed_boost;
    }
    else // Desaceleración natural
    {
        // Aplicamos la desaceleración antes del boost
        float base_speed = speed / speed_boost; // Revertimos el boost temporalmente
        base_speed *= 0.95f;                    // Aplicamos la desaceleración
        if (fabs(base_speed) < 10.0f)
            base_speed = 0.0f;
        speed = base_speed * speed_boost;       // Reaplicamos el boost
    }
    if (App->game->IsPlayer1Winner())
    {
        return UPDATE_CONTINUE; // No dibujar coches si el jugador 1 ganó
    }
    if (App->game->IsGameOver())
    {
        return UPDATE_CONTINUE; // Detener el dibujado de los jugadores si el juego ha terminado
    }
    if (IsKeyDown(KEY_A)) // Girar a la izquierda
    {
        car_rotation -= handling * delta_time;
    }
    if (IsKeyDown(KEY_D)) // Girar a la derecha
    {
        car_rotation += handling * delta_time;
    }

    if (IsKeyDown(KEY_UP))
    {
        player2_speed += acceleration * delta_time;
        if (player2_speed > max_speed)
            player2_speed = max_speed;
        player2_speed *= speed_boost;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        player2_speed -= acceleration * delta_time;
        if (player2_speed < -max_speed / 2)
            player2_speed = -max_speed / 2;
        player2_speed *= speed_boost;
    }
    else // Desaceleración natural
    {
        float base_speed = player2_speed / speed_boost;
        base_speed *= 0.95f;
        if (fabs(base_speed) < 10.0f)
            base_speed = 0.0f;
        player2_speed = base_speed * speed_boost;
    }
    if (IsKeyDown(KEY_LEFT)) // Girar a la izquierda
    {
        player2_rotation -= handling * delta_time;
    }
    if (IsKeyDown(KEY_RIGHT)) // Girar a la derecha
    {
        player2_rotation += handling * delta_time;
    }

    // Ajustar el movimiento para que coincida con la orientación del sprite
    float adjusted_rotation1 = car_rotation - 90.0f;
    float adjusted_rotation2 = player2_rotation - 90.0f;

    // Calcular la posición del jugador 1
    car_position.x -= cos(adjusted_rotation1 * DEG2RAD) * speed * delta_time;
    car_position.y -= sin(adjusted_rotation1 * DEG2RAD) * speed * delta_time;

    car_body->body->SetTransform(
        b2Vec2(PIXEL_TO_METERS(car_position.x), PIXEL_TO_METERS(car_position.y)),
        car_rotation * DEG2RAD
    );

    // Calcular la posición del jugador 2
    player2_position.x -= cos(adjusted_rotation2 * DEG2RAD) * player2_speed * delta_time;
    player2_position.y -= sin(adjusted_rotation2 * DEG2RAD) * player2_speed * delta_time;

    player2_body->body->SetTransform(
        b2Vec2(PIXEL_TO_METERS(player2_position.x), PIXEL_TO_METERS(player2_position.y)),
        player2_rotation * DEG2RAD
    );

    // Escalar el sprite de los coches
    float scale = 0.06f;

    // Dibujar el coche del jugador 1
    DrawTexturePro(
        car_texture,
        Rectangle{ 0.0f, 0.0f, (float)car_texture.width, (float)car_texture.height }, // Toda la textura
        Rectangle{ car_position.x, car_position.y, (float)car_texture.width * scale, (float)car_texture.height * scale }, // Escalado
        Vector2{ (float)car_texture.width * scale / 2.0f, (float)car_texture.height * scale / 2.0f }, // Centro ajustado
        car_rotation,
        WHITE
    );

    // Dibujar el coche del jugador 2
    DrawTexturePro(
        car_texture2,
        Rectangle{ 0.0f, 0.0f, (float)car_texture2.width, (float)car_texture2.height }, // Toda la textura
        Rectangle{ player2_position.x, player2_position.y, (float)car_texture2.width * scale, (float)car_texture2.height * scale }, // Escalado
        Vector2{ (float)car_texture2.width * scale / 2.0f, (float)car_texture2.height * scale / 2.0f }, // Centro ajustado
        player2_rotation,
        WHITE
    );

    return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
    LOG("Unloading player assets");
    UnloadTexture(car_texture);
    UnloadTexture(car_texture2);
    return true;
}

// Métodos públicos para obtener datos
Vector2 ModulePlayer::GetCarPosition() const
{
    return car_position;
}

PhysBody* ModulePlayer::GetCarBody() const
{
    return car_body;
}

PhysBody* ModulePlayer::GetPlayer2Body() const
{
    return player2_body;
}
void ModulePlayer::ApplySpeedBoost(int playerNum)
{
    if (playerNum == 1)
    {
        speed_boost = 1.5f;  // Aumentado el boost para que sea más notable
        LOG("Applied speed boost to Player 1");
    }
    else if (playerNum == 2)
    {
        speed_boost = 1.5f;  // Aumentado el boost para que sea más notable
        LOG("Applied speed boost to Player 2");
    }
}

void ModulePlayer::RestoreSpeedBoost(int playerNum)
{
    if (playerNum == 1 || playerNum == 2)
    {
        speed_boost = base_speed_boost;  // Restaurar a la velocidad base
        LOG("Restored normal speed");
    }
}
