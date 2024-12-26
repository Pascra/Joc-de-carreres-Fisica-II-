#include "ModuleGame.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"

// Constructor
ModuleGame::ModuleGame(Application* app, bool start_enabled)
    : Module(app, start_enabled), map_texture{ 0 }, current_checkpoint(0), laps(0)
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

    // Crear sensores de puntos de control y línea de meta
    checkpoint_sensors.push_back(App->physics->CreateRectangleSensor(200, 100, 50, 50)); // Checkpoint 1
    checkpoint_sensors.push_back(App->physics->CreateRectangleSensor(500, 300, 50, 50)); // Checkpoint 2
    checkpoint_sensors.push_back(App->physics->CreateRectangleSensor(800, 600, 50, 50)); // Checkpoint 3
    finish_line = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50, 100, 10); // Línea de meta

    for (auto sensor : checkpoint_sensors)
    {
        sensor->listener = this; // Escuchar colisiones
    }
    finish_line->listener = this;

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

// Detectar colisiones
void ModuleGame::OnCollision(PhysBody* sensor, PhysBody* other)
{
    LOG("¡El coche ha ganado!");
        
    // Validar si el sensor es parte de la lista de sensores
    if (std::find(checkpoint_sensors.begin(), checkpoint_sensors.end(), sensor) != checkpoint_sensors.end() || sensor == finish_line)
    {
        LOG("¡El coche ha ganado!");
        // Procesar colisión
        if (sensor == finish_line)
        {
            if (current_checkpoint == checkpoint_sensors.size())
            {
                laps++;
                current_checkpoint = 0; // Reiniciar para la siguiente vuelta
                LOG("Vuelta completada. Vueltas: %d", laps);

                if (laps >= 3)
                {
                    LOG("¡El coche ha ganado!");
                    // Cambiar estado del juego a victoria
                }
            }
            else
            {
                LOG("Vuelta no válida. Completa todos los puntos de control.");
            }
        }
        else
        {
            for (size_t i = 0; i < checkpoint_sensors.size(); ++i)
            {
                if (sensor == checkpoint_sensors[i] && i == current_checkpoint)
                {
                    current_checkpoint++;
                    LOG("Checkpoint %d alcanzado", (int)i + 1);
                    break;
                }
            }
        }
    }
    else
    {
        LOG("Sensor no reconocido.");
    }
}


// Unload assets
bool ModuleGame::CleanUp()
{
    LOG("Unloading game assets");

    for (auto sensor : checkpoint_sensors)
    {
        delete sensor; // Asegúrate de liberar memoria si es necesario
    }
    checkpoint_sensors.clear();

    delete finish_line; // Libera la línea de meta

    UnloadTexture(map_texture);
    return true;
}
