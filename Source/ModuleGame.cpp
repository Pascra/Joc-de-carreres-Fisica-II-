#include "ModuleGame.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"
#include <cmath>
#include "ModulePlayer.h"
#include "ModulePhysics.h"


class PhysicEntity
{
protected:

    PhysicEntity(PhysBody* _body, Module* listener)
        : body(_body), listener(listener)
    {
        body->listener = listener;
    }

public:
    virtual ~PhysicEntity() = default;
    virtual void Update() = 0;
    CollisionType ctype;

    virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
    {
        return 0;
    }

protected:
    PhysBody* body;
    Module* listener = nullptr;
};

class Rick : public PhysicEntity
{
public:
    // Pivot 0, 0
    static constexpr int rick_head[40] = {
        104, 140,
        105, 118,
        108, 103,
        114, 88,
        126, 66,
        138, 53,
        155, 37,
        178, 23,
        200, 16,
        218, 13,
        233, 12,
        233, 0,
        192, 0,
        183, 1,
        154, 14,
        132, 30,
        111, 54,
        94, 88,
        88, 119,
        88, 140,

    };

    Rick(ModulePhysics* physics, int _x, int _y, Module* listener, Texture2D _texture)
        : PhysicEntity(physics->CreateChain(0,0, rick_head, 40), listener)
        , texture(_texture)
    {
        ctype = CollisionType::DEFAULT;
    }

    void Update() override
    {
        int x, y;
        body->GetPhysicPosition(x, y);
        DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
    }

private:
    Texture2D texture;
};


class Rick2 : public PhysicEntity
{
public:
    // Pivot 0, 0
    static constexpr int rick_hola[8] = {
        
        320, 60,
        340, 60,
        340, 40,
        320, 40,
        

    };

    Rick2(ModulePhysics* physics, int _x, int _y, Module* listener, Texture2D _texture)
        : PhysicEntity(physics->CreateChain(0, 0, rick_hola, 8), listener)
        , texture(_texture)
    {
       
    }

    void Update() override
    {
        int x, y;
        body->GetPhysicPosition(x, y);
        DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
    }

private:
    Texture2D texture;
};


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
    ai_position({ 913.7f, 471.0f }), // Posici�n inicial
    ai_rotation(0.0f),
    ai_speed(0.0f),
    ai_body(nullptr),
    player1_won(false),
    player2_won(false),
    Ai_won(false)
{
}

// Destructor
ModuleGame::~ModuleGame() {}

// Load assets
bool ModuleGame::Start()
{
    TraceLog(LOG_INFO, "Loading game assets");

    entities.emplace_back(new Rick(App->physics, 0, 0, this, default));
    entities.emplace_back(new Rick2(App->physics, 0, 0, this, default));

    player1_win_texture = LoadTexture("Assets/player1_gana.png");
    if (player1_win_texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load Player 1 win texture!");
        return false;
    }
    player2_win_texture = LoadTexture("Assets/player2_gana.png");
    if (player1_win_texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load Player 2 win texture!");
        return false;
    }
    Ai_win_texture = LoadTexture("Assets/AI_gana.png");
    if (Ai_win_texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Failed to load Ai win texture!");
        return false;
    }

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

    // Definir puntos de control con posiciones y tama�os
    checkpoint_definitions = {
       {887, 200, 100, 16}, {750, 60, 16, 100}, {250, 60, 16, 100},{150, 130, 100, 16}, {400, 382, 16, 100}, {585, 190, 16, 100},
        {645, 382, 100, 16}, {400, 616, 16, 100},{160, 620, 16, 90}, {78, 540, 90, 16}, {210, 455, 16, 100},
        {348, 560, 90, 16}, {344, 670, 90, 16}, {220, 880, 90, 16}, {780, 960, 16, 100},{887, 860, 100, 16}, {887, 598, 100, 16}
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

    // Crear l�nea de meta
    finish_line = App->physics->CreateRectangleSensor(SCREEN_WIDTH - 134, SCREEN_HEIGHT - 600, 100, 16);
    if (!finish_line)
    {
        TraceLog(LOG_ERROR, "Failed to create finish line sensor");
        return false;
    }
    finish_line->listener = this;

    // Configuraci�n inicial de la IA
    ai_position = { 913.7f, 525.0f }; // Cambia estas coordenadas seg�n lo que desees
    ai_rotation = 180.0f;             // Rotaci�n inicial hacia la derecha
    ai_speed = 0.0f;

    // Crear cuerpo f�sico de la IA
    float hitbox_width = 50.0f;  // Ajusta seg�n el tama�o del coche
    float hitbox_height = 30.0f; // Ajusta seg�n el tama�o del coche
    ai_body = App->physics->CreateRectangle(ai_position.x, ai_position.y, hitbox_width, hitbox_height);
    ai_body->body->SetType(b2_dynamicBody); // Configurar como cuerpo din�mico
    ai_body->ctype = CollisionType::AI;
    // Ajustar la rotaci�n inicial
    ai_body->body->SetTransform(
        b2Vec2(PIXEL_TO_METERS(ai_position.x), PIXEL_TO_METERS(ai_position.y)),
        ai_rotation * DEG2RAD);
    //Empieza a contar el tiempo
    time = 0.0f;
    return true;
}

// Update
update_status ModuleGame::Update()
{
    if (player1_won)
    {
        DrawTexturePro(
            player1_win_texture,
            Rectangle{ 0.0f, 0.0f, (float)player1_win_texture.width, (float)player1_win_texture.height },
            Rectangle{ (float)SCREEN_WIDTH / 2 - player1_win_texture.width / 2, (float)SCREEN_HEIGHT / 2 - player1_win_texture.height / 2,
                       (float)player1_win_texture.width, (float)player1_win_texture.height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
        return UPDATE_CONTINUE; // Salir del m�todo sin dibujar otros elementos
    }
    if (player2_won)
    {
        DrawTexturePro(
            player2_win_texture,
            Rectangle{ 0.0f, 0.0f, (float)player2_win_texture.width, (float)player2_win_texture.height },
            Rectangle{ (float)SCREEN_WIDTH / 2 - player2_win_texture.width / 2, (float)SCREEN_HEIGHT / 2 - player2_win_texture.height / 2,
                       (float)player2_win_texture.width, (float)player2_win_texture.height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
        return UPDATE_CONTINUE; // Salir del m�todo sin dibujar otros elementos
    }
    if (Ai_won)
    {
        DrawTexturePro(
            Ai_win_texture,
            Rectangle{ 0.0f, 0.0f, (float)Ai_win_texture.width, (float)Ai_win_texture.height },
            Rectangle{ (float)SCREEN_WIDTH / 2 - Ai_win_texture.width / 2, (float)SCREEN_HEIGHT / 2 - Ai_win_texture.height / 2,
                       (float)Ai_win_texture.width, (float)Ai_win_texture.height },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
        return UPDATE_CONTINUE; // Salir del m�todo sin dibujar otros elementos
    }

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

        // Calcular direcci�n hacia el checkpoint
        float direction_x = checkpoint_x - ai_position.x;
        float direction_y = checkpoint_y - ai_position.y;
        float distance = sqrt(direction_x * direction_x + direction_y * direction_y);

        // Verificar si se ha alcanzado el checkpoint
        if (distance < 50.0f) // Rango de detecci�n ajustado
        {
            current_checkpoint_ai++; // Cambiar al siguiente checkpoint
            if (current_checkpoint_ai >= checkpoint_sensors.size())
            {
                // Reiniciar tras completar una vuelta
                current_checkpoint_ai = 0;
                ai_speed = 0.0f; // Reiniciar velocidad
                ai_rotation = 180.0f; // Reiniciar rotaci�n
                ai_position = { 913.7f, 525.0f }; // Reiniciar posici�n inicial
                ai_body->body->SetTransform(
                    b2Vec2(PIXEL_TO_METERS(ai_position.x), PIXEL_TO_METERS(ai_position.y)),
                    ai_rotation * DEG2RAD);
                laps_ai++;
                TraceLog(LOG_INFO, "AI completed lap %d", laps_ai);
                if (laps_ai >= 3)
                {
                    TraceLog(LOG_INFO, "AI WINS!");
                    Ai_won = true;
                }
            }
        }
        else // Mover hacia el checkpoint
        {
            direction_x /= distance; // Normalizar el vector direcci�n
            direction_y /= distance;

            ai_speed = 250.0f; // Velocidad constante de la IA

            // Calcular la rotaci�n hacia el checkpoint
            float target_rotation = atan2(direction_y, direction_x) * RAD2DEG;

            // Calcular la diferencia m�nima entre el �ngulo actual y el objetivo
            float delta_rotation = fmod(target_rotation - ai_rotation + 360.0f, 360.0f);
            if (delta_rotation > 180.0f)
            {
                delta_rotation -= 360.0f;
            }

            // Ajustar rotaci�n progresivamente
            float rotation_speed = 130.0f * GetFrameTime();
            if (fabs(delta_rotation) > rotation_speed)
            {
                ai_rotation += (delta_rotation > 0) ? rotation_speed : -rotation_speed;
            }
            else
            {
                ai_rotation = target_rotation; // Ajustar directamente si est� cerca
            }

            // Actualizar posici�n
            ai_position.x += cos(ai_rotation * DEG2RAD) * ai_speed * GetFrameTime();
            ai_position.y += sin(ai_rotation * DEG2RAD) * ai_speed * GetFrameTime();

            // Actualizar posici�n f�sica
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
        ai_rotation - 90.0f, // Ajuste de orientaci�n
        WHITE);

    time += GetFrameTime();
    DrawTime();

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
                player1_won = true;
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
                player2_won = true;
                // L�gica adicional si es necesario cuando el Player 2 gana.
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


void ModuleGame::DrawTime() {
    Vector2 position = { 10.0f, 40.0f };
    float fontSize = 20.0f;  // Reduje el tama�o para que se vea mejor
    Color color = BLACK;

    char TimeText[20];
    int minutes = (int)time / 60;
    int seconds = (int)time % 60;

    // Formato MM:SS
    snprintf(TimeText, sizeof(TimeText), "Time: %02d:%02d", minutes, seconds);

    // Usar DrawText de raylib directamente
    DrawText(TimeText, (int)position.x, (int)position.y, (int)fontSize, color);
}

bool ModuleGame::CleanUp()
{
    TraceLog(LOG_INFO, "Unloading game assets");

    for (auto sensor : checkpoint_sensors)
    {
        delete sensor; // Libera sensores
    }
    checkpoint_sensors.clear();

    if (finish_line)
    {
        delete finish_line; // Libera l�nea de meta
        finish_line = nullptr;
    }

    UnloadTexture(map_texture); // Libera textura del mapa
    UnloadTexture(ai_texture);  // Libera textura de la IA

    return true;
}


