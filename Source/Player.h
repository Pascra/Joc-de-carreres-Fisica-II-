#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Entity.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

};

class Player : public Entity {
private:
    std::string name;         // Nombre del jugador
    float speed;              // Velocidad máxima del coche
    float acceleration;       // Aceleración del coche
    float handling;           // Manejo del coche (agilidad al girar)
    int score;                // Puntuación del jugador

public:
    // Constructor
    Player(const std::string& playerName, Vector2 pos, Vector2 vel, float maxSpeed, float accel, float handle, Color clr)
        : Entity(pos, vel, 0.0f, 1.0f, clr), name(playerName), speed(maxSpeed), acceleration(accel), handling(handle), score(0) {}

  
    std::string GetName() const { return name; }
    float GetSpeed() const { return speed; }
    float GetAcceleration() const { return acceleration; }
    float GetHandling() const { return handling; }
    int GetScore() const { return score; }

    
    void SetName(const std::string& playerName) { name = playerName; }
    void SetSpeed(float maxSpeed) { speed = maxSpeed; }
    void SetAcceleration(float accel) { acceleration = accel; }
    void SetHandling(float handle) { handling = handle; }
    void SetScore(int newScore) { score = newScore; }

    // Métodos
    void Update(float deltaTime) override {
        // Movimiento básico considerando aceleración y velocidad máxima
        velocity.x += acceleration * deltaTime;
        if (velocity.x > speed) velocity.x = speed;

        // Rotación/manejo (puedes personalizar con controles)
        if (IsKeyDown(KEY_LEFT)) rotation -= handling * deltaTime;
        if (IsKeyDown(KEY_RIGHT)) rotation += handling * deltaTime;

        // Actualización de la posición
        position.x += velocity.x * deltaTime * cos(rotation * DEG2RAD);
        position.y += velocity.x * deltaTime * sin(rotation * DEG2RAD);
    }

    void Draw() const override {
        // Representación del coche del jugador
        DrawRectanglePro(
            { position.x, position.y, 40.0f * scale, 20.0f * scale },
            { 20.0f * scale, 10.0f * scale },
            rotation, color
        );

        // Dibujar el nombre del jugador encima del coche
        DrawText(name.c_str(), position.x - 20, position.y - 30, 10, WHITE);
    }
};

#endif // PLAYER_H
