#include "ModuleGame.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"
#include <cmath>
#include "ModulePlayer.h"

// Constructor
ModuleGame::ModuleGame(Application* app, bool start_enabled)
    : Module(app, start_enabled),
    map_texture{ 0 },
    current_checkpoint_player1(0),
    laps_player1(0),
    current_checkpoint_player2(0),
    laps_player2(0),
    current_checkpoint_ai(0),
    laps_ai(0),
    ai_position({ 913.7f, 471.0f }), // Posición inicial
    ai_rotation(0.0f),
    ai_speed(0.0f),
    ai_body(nullptr)
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

    // Cargar textura del coche de la IA
    ai_texture = LoadTexture("Assets/cars/pitstop_car_3.png");
    if (ai_texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load AI car texture!");
        return false;
    }

    // Definir puntos de control con posiciones y tamaños
    checkpoint_definitions = {
        {670, 60, 16, 100}, {150, 130, 100, 16}, {400, 382, 16, 100}, {585, 190, 16, 100},
        {645, 382, 100, 16}, {400, 616, 16, 100}, {78, 540, 90, 16}, {210, 455, 16, 100},
        {348, 560, 90, 16}, {344, 670, 90, 16}, {220, 880, 90, 16}, {780, 960, 16, 100}, {887, 598, 100, 16}
    };

    // Crear sensores de puntos de control
    for (const auto& checkpoint : checkpoint_definitions)
    {
        PhysBody* sensor = App->physics->CreateRectangleSensor(
            checkpoint.x, checkpoint.y, checkpoint.width, checkpoint.height);
        if (!sensor)
        {
            TraceLog(LOG_ERROR, "Failed to create checkpoint sensor at (%d, %d)", checkpoint.x, checkpoint.y);
            return false;
        }
        sensor->listener = this;
        checkpoint_sensors.push_back(sensor);
    }

    // Crear línea de meta
    finish_line = App->physics->CreateRectangleSensor(SCREEN_WIDTH - 134, SCREEN_HEIGHT - 600, 100, 16);
    if (!finish_line)
    {
        TraceLog(LOG_ERROR, "Failed to create finish line sensor");
        return false;
    }
    finish_line->listener = this;

    // Configuración inicial de la IA
    ai_position = { 913.7f, 471.0f }; // Cambia estas coordenadas según lo que desees
    ai_rotation = 0.0f;             // Rotación inicial hacia la derecha
    ai_speed = 0.0f;

    // Crear cuerpo físico de la IA
    float hitbox_width = 50.0f;  // Ajusta según el tamaño del coche
    float hitbox_height = 30.0f; // Ajusta según el tamaño del coche
    ai_body = App->physics->CreateRectangle(ai_position.x, ai_position.y, hitbox_width, hitbox_height);
    ai_body->body->SetType(b2_dynamicBody); // Configurar como cuerpo dinámico

    // Ajustar la rotación inicial
    ai_body->body->SetTransform(
        b2Vec2(PIXEL_TO_METERS(ai_position.x), PIXEL_TO_METERS(ai_position.y)),
        ai_rotation * DEG2RAD);

    return true;
}

// Update
update_status ModuleGame::Update()
{
    // Dibujar el mapa
    DrawTexturePro(
        map_texture,
        Rectangle{ 0.0f, 0.0f, (float)map_texture.width, (float)map_texture.height },
        Rectangle{ 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE);

     // Movimiento de la IA
    if (current_checkpoint_ai < checkpoint_sensors.size())
    {
        // Obtener el checkpoint actual
        PhysBody* target_checkpoint = checkpoint_sensors[current_checkpoint_ai];
        int checkpoint_x, checkpoint_y;
        target_checkpoint->GetPhysicPosition(checkpoint_x, checkpoint_y);

        // Calcular dirección hacia el checkpoint
        float direction_x = checkpoint_x - ai_position.x;
        float direction_y = checkpoint_y - ai_position.y;
        float distance = sqrt(direction_x * direction_x + direction_y * direction_y);

        // Verificar si se ha alcanzado el checkpoint
        if (distance < 50.0f) // Rango de detección ajustado
        {
            current_checkpoint_ai++; // Cambiar al siguiente checkpoint
            if (current_checkpoint_ai >= checkpoint_sensors.size())
            {
                // Reiniciar tras completar una vuelta
                current_checkpoint_ai = 0;
                ai_speed = 0.0f; // Reiniciar velocidad
                ai_rotation = 0.0f; // Reiniciar rotación
                ai_position = { 913.7f, 471.0f }; // Reiniciar posición inicial
                ai_body->body->SetTransform(
                    b2Vec2(PIXEL_TO_METERS(ai_position.x), PIXEL_TO_METERS(ai_position.y)),
                    ai_rotation * DEG2RAD);
                laps_ai++;
                TraceLog(LOG_INFO, "AI completed lap %d", laps_ai);
                if (laps_ai >= 3)
                {
                    TraceLog(LOG_INFO, "AI WINS!");
                }
            }
        }
        else // Mover hacia el checkpoint
        {
            direction_x /= distance; // Normalizar el vector dirección
            direction_y /= distance;

            ai_speed = 150.0f; // Velocidad constante de la IA

            // Calcular la rotación hacia el checkpoint
            float target_rotation = atan2(direction_y, direction_x) * RAD2DEG;

            // Ajustar rotación progresivamente
            float rotation_speed = 130.0f * GetFrameTime();
            if (fabs(target_rotation - ai_rotation) > rotation_speed)
            {
                if (target_rotation > ai_rotation)
                {
                    ai_rotation += rotation_speed;
                }
                else
                {
                    ai_rotation -= rotation_speed;
                }
            }
            else
            {
                ai_rotation = target_rotation; // Ajustar directamente si está cerca
            }

            // Actualizar posición
            ai_position.x += cos(ai_rotation * DEG2RAD) * ai_speed * GetFrameTime();
            ai_position.y += sin(ai_rotation * DEG2RAD) * ai_speed * GetFrameTime();

            // Actualizar posición física
            ai_body->body->SetTransform(
                b2Vec2(PIXEL_TO_METERS(ai_position.x), PIXEL_TO_METERS(ai_position.y)),
                ai_rotation * DEG2RAD);
        }
    }

    // Dibujar la textura de la IA
    float scale = 0.06f;
    Vector2 sprite_offset = {
        (float)ai_texture.width * scale / 2.0f,
        (float)ai_texture.height * scale / 2.0f };

    DrawTexturePro(
        ai_texture,
        Rectangle{ 0.0f, 0.0f, (float)ai_texture.width, (float)ai_texture.height },
        Rectangle{ ai_position.x, ai_position.y, (float)ai_texture.width * scale, (float)ai_texture.height * scale },
        sprite_offset,
        ai_rotation - 90.0f, // Ajuste de orientación
        WHITE);

    return UPDATE_CONTINUE;
}

// OnCollision
void ModuleGame::OnCollision(PhysBody* sensor, PhysBody* other)
{
    if (sensor == finish_line)
    {
        // Player 1
        if (other == App->player->GetCarBody() && current_checkpoint_player1 == checkpoint_sensors.size())
        {
            laps_player1++;
            current_checkpoint_player1 = 0;
            TraceLog(LOG_INFO, "Player 1 completed a lap! Total laps: %d", laps_player1);
            if (laps_player1 >= 3)
            {
                TraceLog(LOG_INFO, "Player 1 WINS!");
                // Lógica adicional si es necesario cuando el Player 1 gana.
            }
        }

        // Player 2
        if (other == App->player->GetPlayer2Body() && current_checkpoint_player2 == checkpoint_sensors.size())
        {
            laps_player2++;
            current_checkpoint_player2 = 0;
            TraceLog(LOG_INFO, "Player 2 completed a lap! Total laps: %d", laps_player2);
            if (laps_player2 >= 3)
            {
                TraceLog(LOG_INFO, "Player 2 WINS!");
                // Lógica adicional si es necesario cuando el Player 2 gana.
            }
        }

        
        }
    }

    // Checkpoints
    auto checkpoint_index = std::find(checkpoint_sensors.begin(), checkpoint_sensors.end(), sensor) - checkpoint_sensors.begin();
    if (checkpoint_index < checkpoint_sensors.size())
    {
        // Player 1
        if (other == App->player->GetCarBody() && checkpoint_index == current_checkpoint_player1)
        {
            current_checkpoint_player1++;
          
        }

        // Player 2
        if (other == App->player->GetPlayer2Body() && checkpoint_index == current_checkpoint_player2)
        {
            current_checkpoint_player2++;
           
        }

        // IA
        if (other == ai_body && checkpoint_index == current_checkpoint_ai)
        {
            current_checkpoint_ai++;
            
        }
    }
}



// CleanUp
bool ModuleGame::CleanUp()
{
    TraceLog(LOG_INFO, "Unloading game assets");

    for (auto sensor : checkpoint_sensors)
        delete sensor;

    checkpoint_sensors.clear();
    delete finish_line;
    UnloadTexture(ai_texture);
    return true;
}
