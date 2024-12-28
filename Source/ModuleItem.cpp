#include "ModuleItem.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Globals.h"

ModuleItem::ModuleItem(Application* app, bool start_enabled) : Module(app, start_enabled),
item_active(true), respawn_timer(0.0f), respawn_time(5.0f),
current_frame(0), frames_counter(0), frames_speed(7),
boost_duration(3.0f), boost_timer_p1(0.0f), boost_timer_p2(0.0f),
is_boosted_p1(false), is_boosted_p2(false)
{
}

ModuleItem::~ModuleItem()
{
}

bool ModuleItem::Start()
{
    LOG("Loading item");

    // Cargar textura del item
    item_texture = LoadTexture("Assets/nitro.png");

    // Configurar el rectángulo de la animación
    frame_rec.x = 0;
    frame_rec.y = 0;
    frame_rec.width = (float)item_texture.width / 7;
    frame_rec.height = (float)item_texture.height;

    // Posición inicial del item
    item_position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    // Crear el sensor físico
    item_sensor = App->physics->CreateRectangleSensor(
        item_position.x,
        item_position.y,
        frame_rec.width * 0.5f,  // Reducido el tamaño del sensor
        frame_rec.height * 0.5f
    );
    item_sensor->ctype = CollisionType::ITEM;
    item_sensor->listener = this; // ¡IMPORTANTE! Esto faltaba

    return true;
}

update_status ModuleItem::Update()
{
    float delta_time = GetFrameTime();

    // Actualizar temporizadores de boost
    UpdateBoostTimers();

    // Si el item está desactivado, actualizar el temporizador de reaparición
    if (!item_active)
    {
        respawn_timer += delta_time;
        if (respawn_timer >= respawn_time)
        {
            item_active = true;
            respawn_timer = 0.0f;
            // Reactivar el sensor
            if (item_sensor && item_sensor->body)
            {
                item_sensor->body->GetFixtureList()->SetSensor(false);
            }
        }
    }

    // Si el item está activo, actualizar la animación y dibujarlo
    if (item_active)
    {
        frames_counter++;
        if (frames_counter >= (60 / frames_speed))
        {
            frames_counter = 0;
            current_frame++;

            if (current_frame > 7)
                current_frame = 0;

            frame_rec.x = (float)current_frame * frame_rec.width;
        }

        // Dibujar el item solo si está activo
        DrawTexturePro(
            item_texture,
            frame_rec,
            Rectangle{
                item_position.x,
                item_position.y,
                frame_rec.width,
                frame_rec.height
            },
            Vector2{ frame_rec.width / 2.0f, frame_rec.height / 2.0f },
            0.0f,
            WHITE
        );
    }

    return UPDATE_CONTINUE;
}

void ModuleItem::UpdateBoostTimers()
{
    float delta_time = GetFrameTime();

    // Actualizar boost del player 1
    if (is_boosted_p1)
    {
        boost_timer_p1 += delta_time;
        if (boost_timer_p1 >= boost_duration)
        {
            is_boosted_p1 = false;
            boost_timer_p1 = 0.0f;
            App->player->RestoreSpeedBoost(1);
            LOG("Player 1 boost ended");
        }
    }

    // Actualizar boost del player 2
    if (is_boosted_p2)
    {
        boost_timer_p2 += delta_time;
        if (boost_timer_p2 >= boost_duration)
        {
            is_boosted_p2 = false;
            boost_timer_p2 = 0.0f;
            App->player->RestoreSpeedBoost(2);
            LOG("Player 2 boost ended");
        }
    }
}

void ModuleItem::OnCollision(PhysBody* body1, PhysBody* body2)
{
    // Verificar que el item esté activo
    if (!item_active) return;

    PhysBody* other_body = nullptr;

    // Determinar cuál es el cuerpo del item y cuál es el otro
    if (body1 == item_sensor)
        other_body = body2;
    else if (body2 == item_sensor)
        other_body = body1;
    else
        return;

    // Verificar la colisión con los jugadores
    if (other_body->ctype == CollisionType::PLAYER1)
    {
        LOG("Player 1 collected boost!");
        ApplyBoostToPlayer(1);
    }
    else if (other_body->ctype == CollisionType::PLAYER2)
    {
        LOG("Player 2 collected boost!");
        ApplyBoostToPlayer(2);
    }
}

void ModuleItem::ApplyBoostToPlayer(int playerNum)
{
    if (playerNum == 1)
    {
        is_boosted_p1 = true;
        boost_timer_p1 = 0.0f;
        App->player->ApplySpeedBoost(1);
    }
    else if (playerNum == 2)
    {
        is_boosted_p2 = true;
        boost_timer_p2 = 0.0f;
        App->player->ApplySpeedBoost(2);
    }

    // Desactivar el item
    item_active = false;
    respawn_timer = 0.0f;

    // Desactivar el sensor
    if (item_sensor && item_sensor->body)
    {
        item_sensor->body->GetFixtureList()->SetSensor(true);
    }

    LOG("Item collected and deactivated");
}

bool ModuleItem::CleanUp()
{
    LOG("Unloading item");
    UnloadTexture(item_texture);
    return true;
}