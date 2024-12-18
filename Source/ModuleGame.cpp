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
    car_texture = LoadTexture("Assets/coche.png");
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
    // Dibujar mapa
    DrawTexture(map_texture, 0, 0, WHITE);
    DrawTexture(car_texture, 0, 0, WHITE);

    return UPDATE_CONTINUE;
}

// Unload assets
bool ModuleGame::CleanUp()
{
    LOG("Unloading game assets");
    UnloadTexture(map_texture);
    return true;
}
