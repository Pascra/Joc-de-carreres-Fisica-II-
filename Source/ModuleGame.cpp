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

    map_texture = LoadTexture("Assets/MapaCarreras.png");

    if (map_texture.id == 0)
    {
        LOG("ERROR: Failed to load map texture!");
        return false;
    }

    LOG("Map texture loaded successfully. Dimensions: %d x %d", map_texture.width, map_texture.height);
    return true;

}

// Update
update_status ModuleGame::Update()
{
    // Escalar el mapa para que ocupe toda la ventana
    float scale_x = (float)SCREEN_WIDTH / (float)map_texture.width;
    float scale_y = (float)SCREEN_HEIGHT / (float)map_texture.height;

    DrawTexturePro(
        map_texture,
        Rectangle{ 0.0f, 0.0f, (float)map_texture.width, (float)map_texture.height }, // Área fuente
        Rectangle{ 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },         // Área destino
        Vector2{ 0.0f, 0.0f }, // Sin desplazamiento
        0.0f,                  // Sin rotación
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
