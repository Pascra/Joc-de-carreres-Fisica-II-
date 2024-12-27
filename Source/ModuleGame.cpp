#include "ModuleGame.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"
#include <iostream>


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
    TraceLog(LOG_INFO, "Loading game assets");

    // Cargar la textura del mapa
    map_texture = LoadTexture("Assets/MapaCarreras.png");
    if (map_texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load map texture!");
        return false;
    }
    TraceLog(LOG_INFO, "Map texture loaded successfully. Dimensions: %d x %d", map_texture.width, map_texture.height);

    // Definiciones de puntos de control con posiciones y tamaños
    std::vector<Checkpoint> checkpoint_definitions = {
        {90, 800, 16, 100},  // Checkpoint 1
        {500, 300, 60, 40},  // Checkpoint 2
        {800, 600, 70, 30}   // Checkpoint 3
    };

    // Crear sensores de puntos de control
    for (const auto& checkpoint : checkpoint_definitions)
    {
        PhysBody* sensor = App->physics->CreateRectangleSensor(
            checkpoint.x, checkpoint.y, checkpoint.width, checkpoint.height
        );

        if (!sensor)
        {
            TraceLog(LOG_ERROR, "Failed to create checkpoint sensor at (%d, %d)", checkpoint.x, checkpoint.y);
            return false;
        }

        sensor->listener = this; // Asignar listener al sensor
        checkpoint_sensors.push_back(sensor);
        TraceLog(LOG_INFO, "Checkpoint sensor created at (%d, %d) with size (%d x %d)",
            checkpoint.x, checkpoint.y, checkpoint.width, checkpoint.height);
    }

    // Crear línea de meta
    finish_line = App->physics->CreateRectangleSensor(SCREEN_WIDTH - 134, SCREEN_HEIGHT - 600, 100, 16);
    if (!finish_line)
    {
        TraceLog(LOG_ERROR, "Failed to create finish line sensor");
        return false;
    }
    finish_line->listener = this;
    TraceLog(LOG_INFO, "Finish line sensor created at (%d, %d)", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50);

    // Crear cuerpo de prueba para colisiones
    PhysBody* testBody = App->physics->CreateCircle(400, 300, 20);
    if (testBody)
    {
        testBody->body->SetType(b2_dynamicBody);
        testBody->body->SetLinearVelocity(b2Vec2(5, 0)); // Mover el cuerpo hacia la derecha
        TraceLog(LOG_INFO, "Test body created at (400, 300) and moving right.");
    }
    else
    {
        TraceLog(LOG_WARNING, "Failed to create test body for collision testing");
    }

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
    TraceLog(LOG_INFO, "Collision detected: sensor=%p, finish_line=%p", sensor, finish_line);

    if (sensor == finish_line)
    {
        TraceLog(LOG_INFO, "Finish line triggered");
        if (current_checkpoint == checkpoint_sensors.size())
        {
            laps++;
            current_checkpoint = 0;
            TraceLog(LOG_INFO, "Lap completed. Total laps: %d", laps);

            if (laps >= 3)
            {
                TraceLog(LOG_INFO, "Race completed! Car wins!");
            }
        }
        else
        {
            TraceLog(LOG_WARNING, "Invalid lap. Complete all checkpoints.");
        }
    }
    else if (std::find(checkpoint_sensors.begin(), checkpoint_sensors.end(), sensor) != checkpoint_sensors.end())
    {
        TraceLog(LOG_INFO, "Checkpoint sensor triggered");
        for (size_t i = 0; i < checkpoint_sensors.size(); ++i)
        {
            if (sensor == checkpoint_sensors[i] && i == current_checkpoint)
            {
                current_checkpoint++;
                TraceLog(LOG_INFO, "Checkpoint %d reached", (int)i + 1);
                break;
            }
        }
    }
    else
    {
        TraceLog(LOG_WARNING, "Unrecognized sensor collision. sensor=%p", sensor);
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
