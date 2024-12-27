#include "ModuleItem.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"

ModuleItem::ModuleItem(Application* app, bool start_enabled)
    : Module(app, start_enabled),
    item_texture{ 0 },
    item_position{ 400.0f, 700.0f },
    item_body(nullptr),
    is_visible(true),
    needs_respawn(false),
    boost_active(false),
    respawn_time(3.0f)
{
    // Inicializar la estructura de animación
    nitro.frames = nullptr;
    nitro.frame_count = 0;
    nitro.current_frame = 0;
    nitro.frame_speed = 0.13f; // frames por segundo
    nitro.accumulated_time = 0.0f;
}

ModuleItem::~ModuleItem()
{
    if (nitro.frames != nullptr)
    {
        delete[] nitro.frames;
    }
}

void ModuleItem::InitializeAnimation()
{
    // Configurar los frames de la animación
    // Asumiendo que el spritesheet tiene 6 frames horizontales
    nitro.frame_count = 7;
    nitro.frames = new Rectangle[nitro.frame_count];

    float frame_width = (float)item_texture.width / nitro.frame_count;
    float frame_height = (float)item_texture.height;

    // Definir los rectángulos de cada frame
    for (int i = 0; i < nitro.frame_count; i++)
    {
        nitro.frames[i].x = i * frame_width;
        nitro.frames[i].y = 0;
        nitro.frames[i].width = frame_width;
        nitro.frames[i].height = frame_height;
    }
}

bool ModuleItem::Start()
{
    LOG("Loading item assets");

    item_texture = LoadTexture("Assets/nitro.png");
    if (item_texture.id == 0)
    {
        LOG("ERROR: Failed to load item texture!");
        return false;
    }

    InitializeAnimation();

    // Create physics body for the item
    float scale = 0.2f;
    item_body = App->physics->CreateCircle(
        item_position.x,
        item_position.y,
        (item_texture.width / nitro.frame_count * scale) / 2
    );
    item_body->ctype = CollisionType::ITEM;
    item_body->body->SetType(b2_staticBody);
    item_body->listener = this;
    

    return true;
}

void ModuleItem::UpdateAnimation()
{
    float deltaTime = GetFrameTime();

    // Actualizar el tiempo acumulado
    nitro.accumulated_time += deltaTime;

    // Cambiar al siguiente frame si es necesario
    if (nitro.accumulated_time >= nitro.frame_speed)
    {
        nitro.current_frame++;
        if (nitro.current_frame >= nitro.frame_count)
            nitro.current_frame = 0;

        nitro.accumulated_time = 0.0f;
    }

    
}

update_status ModuleItem::Update()
{

    // Check if item needs to respawn
    if (needs_respawn && respawn_timer.ReadSec() >= respawn_time)
    {
        RespawnItem();
    }

    // Update animation
    if (is_visible)
    {
        UpdateAnimation();
    }

    // Only draw if visible
    if (is_visible)
    {
        float scale = 0.2f;
        float frameWidth = (float)item_texture.width / nitro.frame_count;

        // Draw the current frame of the animation
        DrawTexturePro(
            item_texture,
            nitro.frames[nitro.current_frame],
            Rectangle{
                item_position.x,
                item_position.y,
                frameWidth * scale,
                (float)item_texture.height * scale
            },
            Vector2{
                frameWidth * scale / 2.0f,
                (float)item_texture.height * scale / 2.0f
            },
            0, 
            WHITE
        );
    }

    return UPDATE_CONTINUE;
}



void ModuleItem::RespawnItem()
{
    // Make item visible again
    is_visible = true;
    needs_respawn = false;

    

    // You could randomize the position here if desired
    item_position = Vector2{ 400.0f, 200.0f };  // Or any other position

    // Update physics body position
    item_body->body->SetTransform(
        b2Vec2(PIXEL_TO_METERS(item_position.x), PIXEL_TO_METERS(item_position.y)),
        0.0f
    );
}

bool ModuleItem::CleanUp()
{
    LOG("Unloading item assets");
    UnloadTexture(item_texture);
    if (nitro.frames != nullptr)
    {
        delete[] nitro.frames;
        nitro.frames = nullptr;
    }
    return true;
}


