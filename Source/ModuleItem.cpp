#include "ModuleItem.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Globals.h"
#include "ModuleGame.h"
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

    item_texture = LoadTexture("Assets/nitro.png");

    frame_rec.x = 0;
    frame_rec.y = 0;
    frame_rec.width = (float)item_texture.width / 7;
    frame_rec.height = (float)item_texture.height;

    // Item 1 en el centro
    item_position = { SCREEN_WIDTH / 1.70f, SCREEN_HEIGHT / 2.3f };
    item_sensor = App->physics->CreateRectangleSensor(
        item_position.x,
        item_position.y + 10,
        frame_rec.width * 0.63f,
        frame_rec.height * 0.63f
    );
    item_sensor->ctype = CollisionType::ITEM;
    item_sensor->listener = this;

    // Item 2 en la parte inferior
    item_position2 = { SCREEN_WIDTH / 2.4f, SCREEN_HEIGHT - 100.0f };
    item_sensor2 = App->physics->CreateRectangleSensor(
        item_position2.x,
        item_position2.y + 10,
        frame_rec.width * 0.63f,
        frame_rec.height * 0.63f
    );
    item_sensor2->ctype = CollisionType::ITEM;
    item_sensor2->listener = this;

    item_active = true;
    item_active2 = true;

    return true;
}

update_status ModuleItem::Update()
{
    float delta_time = GetFrameTime();

    UpdateBoostTimers();

    // Respawn item 1
    if (!item_active)
    {
        respawn_timer += delta_time;
        if (respawn_timer >= respawn_time)
        {
            item_active = true;
            respawn_timer = 0.0f;
            if (item_sensor && item_sensor->body)
            {
                item_sensor->body->GetFixtureList()->SetSensor(false);
            }
        }
    }

    // Respawn item 2
    if (!item_active2)
    {
        respawn_timer2 += delta_time;
        if (respawn_timer2 >= respawn_time)
        {
            item_active2 = true;
            respawn_timer2 = 0.0f;
            if (item_sensor2 && item_sensor2->body)
            {
                item_sensor2->body->GetFixtureList()->SetSensor(false);
            }
        }
    }

    if (App->game->IsGameOver())
    {
        return UPDATE_CONTINUE;
    }

    // Actualizar animación
    frames_counter++;
    if (frames_counter >= (60 / frames_speed))
    {
        frames_counter = 0;
        current_frame++;
        if (current_frame > 7) current_frame = 0;
        frame_rec.x = (float)current_frame * frame_rec.width;
    }

    // Dibujar item 1
    if (item_active)
    {
        DrawTexturePro(item_texture,
            frame_rec,
            Rectangle{ item_position.x, item_position.y, frame_rec.width, frame_rec.height },
            Vector2{ frame_rec.width / 2.0f, frame_rec.height / 2.0f },
            0.0f,
            WHITE
        );
    }

    // Dibujar item 2
    if (item_active2)
    {
        DrawTexturePro(item_texture,
            frame_rec,
            Rectangle{ item_position2.x, item_position2.y, frame_rec.width, frame_rec.height },
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
    if (!item_active && !item_active2) return;

    PhysBody* other_body = nullptr;
    bool is_item1 = false;
    bool is_item2 = false;

    // Identificar el item
    if (body1 == item_sensor || body2 == item_sensor)
    {
        other_body = (body1 == item_sensor) ? body2 : body1;
        is_item1 = true;
    }
    else if (body1 == item_sensor2 || body2 == item_sensor2)
    {
        other_body = (body1 == item_sensor2) ? body2 : body1;
        is_item2 = true;
    }

    if (!other_body) return;

    // Procesar colisión para Player 1
    if (other_body->ctype == CollisionType::PLAYER1)
    {
        if (is_item1 && item_active)
        {
            item_active = false;
            respawn_timer = 0.0f;
            if (item_sensor) item_sensor->body->GetFixtureList()->SetSensor(true);
            ApplyBoostToPlayer(1);
        }
        else if (is_item2 && item_active2)
        {
            item_active2 = false;
            respawn_timer2 = 0.0f;
            if (item_sensor2) item_sensor2->body->GetFixtureList()->SetSensor(true);
            ApplyBoostToPlayer(1);
        }
    }
    // Procesar colisión para Player 2
    else if (other_body->ctype == CollisionType::PLAYER2)
    {
        if (is_item1 && item_active)
        {
            item_active = false;
            respawn_timer = 0.0f;
            if (item_sensor) item_sensor->body->GetFixtureList()->SetSensor(true);
            ApplyBoostToPlayer(2);
        }
        else if (is_item2 && item_active2)
        {
            item_active2 = false;
            respawn_timer2 = 0.0f;
            if (item_sensor2) item_sensor2->body->GetFixtureList()->SetSensor(true);
            ApplyBoostToPlayer(2);
        }
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