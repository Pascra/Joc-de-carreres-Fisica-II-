#include "ModuleGame.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"

// Constructor
ModuleGame::ModuleGame(Application* app, bool start_enabled)
    : Module(app, start_enabled), map_texture{ 0 }
{
}

// Destructor
ModuleGame::~ModuleGame() {}

// Load assets
bool ModuleGame::Start()
{
    LOG("Loading game assets");

    map_texture = LoadTexture("Assets/circuitofinal.png");
    if (map_texture.id == 0)
    {
        LOG("ERROR: Failed to load map texture!");
        return false;
    }

    LOG("Map texture loaded successfully.");
    return true;
}

// Update
update_status ModuleGame::Update()
{
    // Escalar el mapa para que ocupe toda la ventana
    float scale_x = (float)SCREEN_WIDTH / 754.0f;
    float scale_y = (float)SCREEN_HEIGHT / 426.0f;

    DrawTexturePro(
        map_texture,
        Rectangle{ 0.0f, 0.0f, 754.0f, 426.0f },
        Rectangle{ 0.0f, 0.0f, 754.0f * scale_x, 426.0f * scale_y },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );

    return UPDATE_CONTINUE;
}


// Unload assets
bool ModuleGame::CleanUp()
{
    LOG("Unloading game assets");
    UnloadTexture(map_texture);
    return true;
}
