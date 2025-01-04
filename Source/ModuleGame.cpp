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

class Borde1 : public PhysicEntity
{
public:
    // Pivot 0, 0
    static constexpr int borde1[348] = {
        301, 1021,
        804, 1022,
        809, 1020,
        820, 1017,
        831, 1013,
        845, 1009,
        852, 1004,
        873, 996,
        885, 988,
        899, 974,
908, 960,
924, 937,
940, 912,
945, 896,
950, 887,
957, 860,
959, 833,
958, 820,
958, 183,
957, 173,
956, 144,
949, 125,
941, 103,
926, 79,
909, 61,
891, 41,
874, 27,
856, 17,
838, 8,
818, 2,
323, 2,
184, 2,
166, 8,
154, 16,
132, 30,
118, 48,
106, 65,
100, 79,
94, 96,
91, 107,
89, 121,
89, 140,
89, 159,
91, 171,
93, 184,
99, 197,
105, 211,
116, 229,
126, 240,
145, 260,
176, 287,
192, 303,
208, 323,
247, 362,
283, 395,
293, 396,
305, 398,
315, 403,
332, 411,
344, 418,
360, 425,
379, 444,
399, 443,
414, 442,
434, 440,
444, 436,
458, 430,
474, 421,
493, 406,
505, 393,
513, 381,
521, 366,
528, 348,
533, 326,
534, 321,
534, 300,
535, 284,
535, 266,
539, 258,
547, 252,
552, 250,
603, 250,
618, 250,
625, 253,
632, 262,
631, 271,
628, 281,
621, 287,
610, 301,
601, 318,
595, 339,
590, 361,
586, 368,
584, 378,
578, 387,
577, 396,
568, 401,
568, 409,
558, 420,
506, 472,
481, 496,
465, 513,
434, 545,
424, 560,
420, 566,
417, 552,
409, 549,
408, 534,
407, 527,
405, 512,
401, 495,
397, 482,
390, 470,
384, 459,
375, 447,
366, 437,
357, 429,
346, 424,
325, 413,
308, 404,
296, 401,
292, 400,
264, 400,
257, 395,
170, 395,
164, 399,
141, 400,
124, 400,
98, 408,
81, 415,
62, 426,
56, 433,
39, 450,
26, 467,
17, 489,
11, 512,
9, 523,
9, 541,
10, 565,
11, 570,
16, 593,
26, 614,
35, 629,
46, 642,
59, 655,
72, 663,
90, 673,
108, 680,
125, 684,
138, 685,
154, 685,
256, 684,
264, 688,
273, 689,
272, 697,
277, 707,
258, 726,
238, 743,
209, 767,
179, 796,
163, 821,
156, 846,
154, 874,
154, 897,
157, 915,
165, 939,
180, 965,
192, 980,
211, 997,
230, 1008,
248, 1014,
273, 1019,
287, 1021,
298, 1021

    };

    Borde1(ModulePhysics* physics, int _x, int _y, Module* listener, Texture2D _texture)
        : PhysicEntity(physics->CreateChain(0,0, borde1, 348), listener)
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

class Borde2 : public PhysicEntity
{
public:
    // Pivot 0, 0
    static constexpr int borde2[228] = {
        281, 878,
    283, 870,
    298, 856,
    322, 837,
    335, 826,
    358, 800,
    382, 772,
    393, 755,
    401, 735,
    406, 727,
    406, 701,
    401, 692,
    402, 675,
    414, 674,
    425, 678,
    454, 678,
    469, 673,
    497, 654,
    514, 642,
    524, 632,
    566, 591,
    605, 553,
    631, 526,
    660, 493,
    681, 467,
    690, 456,
    700, 433,
    707, 403,
    714, 398,
    713, 387,
    721, 379,
    726, 361,
    731, 350,
    740, 339,
    748, 325,
    758, 297,
    760, 267,
    759, 247,
    755, 224,
    747, 203,
    733, 179,
    719, 161,
    699, 146,
    676, 134,
    650, 127,
    628, 124,
    608, 124,
    556, 124,
    544, 124,
    527, 126,
    513, 127,
    496, 133,
    479, 141,
    462, 151,
    452, 160,
    442, 170,
    433, 182,
    424, 196,
    418, 209,
    413, 221,
    411, 239,
    408, 254,
    408, 268,
    401, 273,
    397, 278,
    397, 288,
    407, 297,
    408, 303,
    403, 311,
    396, 317,
    389, 318,
    382, 315,
    368, 300,
    350, 281,
    244, 176,
    224, 156,
    216, 141,
    217, 136,
    222, 129,
    230, 124,
    240, 123,
    251, 122,
    456, 122,
    555, 121,
    642, 121,
    716, 122,
    765, 123,
    787, 127,
    805, 140,
    815, 150,
    823, 163,
    827, 180,
    828, 353,
    829, 390,
    829, 428,
    830, 469,
    831, 537,
    832, 649,
    832, 807,
    832, 837,
    829, 852,
    818, 869,
    814, 877,
    804, 885,
    795, 891,
    782, 894,
    775, 895,
    752, 895,
    674, 896,
    512, 896,
    411, 896,
    300, 896,
    289, 889,
    283, 883


    };

    Borde2(ModulePhysics* physics, int _x, int _y, Module* listener, Texture2D _texture)
        : PhysicEntity(physics->CreateChain(0, 0, borde2, 228), listener)
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

    entities.emplace_back(new Borde1(App->physics, 0, 0, this, default));
    entities.emplace_back(new Borde2(App->physics, 0, 0, this, default));

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

    // Definir puntos de control con posiciones y tamaños
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

    // Crear línea de meta
    finish_line = App->physics->CreateRectangleSensor(SCREEN_WIDTH - 134, SCREEN_HEIGHT - 600, 100, 16);
    if (!finish_line)
    {
        TraceLog(LOG_ERROR, "Failed to create finish line sensor");
        return false;
    }
    finish_line->listener = this;

    // Configuración inicial de la IA
    ai_position = { 913.7f, 525.0f }; // Cambia estas coordenadas según lo que desees
    ai_rotation = 270.0f;             // Rotación inicial hacia la derecha
    ai_speed = 0.0f;

    // Crear cuerpo físico de la IA
    float hitbox_width = 50.0f;  // Ajusta según el tamaño del coche
    float hitbox_height = 30.0f; // Ajusta según el tamaño del coche

    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(PIXEL_TO_METERS(ai_position.x), PIXEL_TO_METERS(ai_position.y));
    body_def.angle = ai_rotation * DEG2RAD; // Rotación inicial en radianes

    b2PolygonShape hitbox;
    hitbox.SetAsBox(PIXEL_TO_METERS(hitbox_width * 0.5f), PIXEL_TO_METERS(hitbox_height * 0.5f));

    b2FixtureDef fixture_def;
    fixture_def.shape = &hitbox;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.3f;
    fixture_def.restitution = 0.0f;

    ai_body = App->physics->CreateRectangle(ai_position.x, ai_position.y, hitbox_width, hitbox_height);
    ai_body->body->SetGravityScale(0); // Configurar gravedad para que no afecte a la IA
    ai_body->ctype = CollisionType::AI;

    // Establecer velocidades iniciales a 0
    ai_body->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    ai_body->body->SetAngularVelocity(0.0f);

    // Empieza a contar el tiempo
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
        return UPDATE_CONTINUE; // Salir del método sin dibujar otros elementos
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
        return UPDATE_CONTINUE; // Salir del método sin dibujar otros elementos
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
        return UPDATE_CONTINUE; // Salir del método sin dibujar otros elementos
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
                ai_rotation = 270.0f; // Reiniciar rotación
                ai_position = { 913.7f, 525.0f }; // Reiniciar posición inicial
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
            direction_x /= distance; // Normalizar el vector dirección
            direction_y /= distance;

            ai_speed = 250.0f; // Velocidad constante de la IA

            // Calcular la rotación hacia el checkpoint
            float target_rotation = atan2(direction_y, direction_x) * RAD2DEG;

            // Calcular la diferencia mínima entre el ángulo actual y el objetivo
            float delta_rotation = fmod(target_rotation - ai_rotation + 360.0f, 360.0f);
            if (delta_rotation > 180.0f)
            {
                delta_rotation -= 360.0f;
            }

            // Ajustar rotación progresivamente
            float rotation_speed = 150.0f * GetFrameTime();
            if (fabs(delta_rotation) > rotation_speed)
            {
                ai_rotation += (delta_rotation > 0) ? rotation_speed : -rotation_speed;
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

    time += GetFrameTime();
    DrawTime();
    DrawLaps(); // Mostrar las vueltas completadas

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
                // Lógica adicional si es necesario cuando el Player 2 gana.
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
    float fontSize = 20.0f;  // Reduje el tamaño para que se vea mejor
    Color color = BLACK;

    char TimeText[20];
    int minutes = (int)time / 60;
    int seconds = (int)time % 60;

    // Formato MM:SS
    snprintf(TimeText, sizeof(TimeText), "Time: %02d:%02d", minutes, seconds);

    // Usar DrawText de raylib directamente
    DrawText(TimeText, (int)position.x, (int)position.y, (int)fontSize, color);
}
void ModuleGame::DrawLaps() {
    float fontSize = 20.0f;

    // Define el color gris
    Color color = GRAY; 

    int startX = SCREEN_WIDTH - 200;
    int startY = 10;
    int lineSpacing = 25;

    char lapsText[50];

    snprintf(lapsText, sizeof(lapsText), "Player 1 Laps: %d", laps_player1);
    DrawText(lapsText, startX, startY, (int)fontSize, color);

    snprintf(lapsText, sizeof(lapsText), "Player 2 Laps: %d", laps_player2);
    DrawText(lapsText, startX, startY + lineSpacing, (int)fontSize, color);

    snprintf(lapsText, sizeof(lapsText), "AI Laps: %d", laps_ai);
    DrawText(lapsText, startX, startY + lineSpacing * 2, (int)fontSize, color);
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
        delete finish_line; // Libera línea de meta
        finish_line = nullptr;
    }

    UnloadTexture(map_texture); // Libera textura del mapa
    UnloadTexture(ai_texture);  // Libera textura de la IA

    return true;
}


