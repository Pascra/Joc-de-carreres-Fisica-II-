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
    acceleration(125.0f), max_speed(300.0f), handling(200.0f), car_body(nullptr), player2_body(nullptr),
    is_drifting(false),
    drift_factor(0.0f),
    drift_angle(0.0f),
    drift_recovery(2.0f),
    drift_speed_multiplier(0.85f),
    lateral_velocity(10.0f)

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
        car_texture2.width * scale * 0.2f,
        car_texture2.height * scale * 0.2f
    );
    car_body->ctype = CollisionType::PLAYER1;
    car_rotation = 180.0f;
    speed = 0.0f;
    car_body->body->SetGravityScale(0);

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
    player2_body->body->SetGravityScale(0);


    LOG("Players initialized.");

    return true;
}

// Update: Lógica de movimiento y dibujo del coche
update_status ModulePlayer::Update()
{
    float delta_time = GetFrameTime();

    // Player 1
    float car_angle = car_body->body->GetAngle();
    float front_angle = car_angle - (90.0f * DEG2RAD);

    // Control del Player 1
    if (IsKeyDown(KEY_W)) // Acelerar hacia adelante
    {
        speed += acceleration * delta_time;
        if (speed > max_speed)
            speed = max_speed;
    }
    else if (IsKeyDown(KEY_S)) // Retroceder
    {
        speed -= acceleration * delta_time;
        if (speed < -max_speed / 2)
            speed = -max_speed / 2;
    }
    else
    {
        speed *= 0.95f; // Desaceleración natural
        if (fabs(speed) < 10.0f)
            speed = 0.0f;
    }

    // Rotación Player 1
    float angular_velocity = 0.0f;
    if (IsKeyDown(KEY_A))
        angular_velocity = -handling;
    if (IsKeyDown(KEY_D))
        angular_velocity = handling;

    car_body->body->SetAngularVelocity(angular_velocity * delta_time);

    b2Vec2 velocity(
        cos(front_angle) * PIXEL_TO_METERS(speed),
        sin(front_angle) * PIXEL_TO_METERS(speed)
    );
    car_body->body->SetLinearVelocity(velocity);

    // Player 2
    float player2_angle = player2_body->body->GetAngle();
    float player2_front_angle = player2_angle - (90.0f * DEG2RAD);

    // Control del Player 2
    if (IsKeyDown(KEY_UP))
    {
        player2_speed += acceleration * delta_time;
        if (player2_speed > max_speed)
            player2_speed = max_speed;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        player2_speed -= acceleration * delta_time;
        if (player2_speed < -max_speed / 2)
            player2_speed = -max_speed / 2;
    }
    else
    {
        player2_speed *= 0.95f;
        if (fabs(player2_speed) < 10.0f)
            player2_speed = 0.0f;
    }

    // Rotación Player 2
    float player2_angular_velocity = 0.0f;
    if (IsKeyDown(KEY_LEFT))
        player2_angular_velocity = -handling;
    if (IsKeyDown(KEY_RIGHT))
        player2_angular_velocity = handling;

    player2_body->body->SetAngularVelocity(player2_angular_velocity * delta_time);

    b2Vec2 player2_velocity(
        cos(player2_front_angle) * PIXEL_TO_METERS(player2_speed),
        sin(player2_front_angle) * PIXEL_TO_METERS(player2_speed)
    );
    player2_body->body->SetLinearVelocity(player2_velocity);

    return UPDATE_CONTINUE;
}



update_status ModulePlayer::PostUpdate()
{
    // Player 1
    b2Vec2 physicsPosition = car_body->body->GetPosition();
    car_position.x = METERS_TO_PIXELS(physicsPosition.x);
    car_position.y = METERS_TO_PIXELS(physicsPosition.y);
    car_rotation = car_body->body->GetAngle() * RAD2DEG;

    float adjusted_rotation = car_rotation + 180.0f;

    float scale = 0.06f;
    DrawTexturePro(
        car_texture,
        Rectangle{ 0.0f, 0.0f, (float)car_texture.width, (float)car_texture.height },
        Rectangle{ car_position.x, car_position.y, (float)car_texture.width * scale, (float)car_texture.height * scale },
        Vector2{ (float)car_texture.width * scale / 2.0f, (float)car_texture.height * scale / 2.0f },
        adjusted_rotation,
        WHITE
    );

    // Player 2
    b2Vec2 player2_physicsPosition = player2_body->body->GetPosition();
    player2_position.x = METERS_TO_PIXELS(player2_physicsPosition.x);
    player2_position.y = METERS_TO_PIXELS(player2_physicsPosition.y);
    player2_rotation = player2_body->body->GetAngle() * RAD2DEG;

    float player2_adjusted_rotation = player2_rotation + 180.0f;

    DrawTexturePro(
        car_texture2,
        Rectangle{ 0.0f, 0.0f, (float)car_texture2.width, (float)car_texture2.height },
        Rectangle{ player2_position.x, player2_position.y, (float)car_texture2.width * scale, (float)car_texture2.height * scale },
        Vector2{ (float)car_texture2.width * scale / 2.0f, (float)car_texture2.height * scale / 2.0f },
        player2_adjusted_rotation,
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
