#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include "Module.h"

class Entity {
protected:
    Vector2 position;       // Posición de la entidad
    Vector2 velocity;       // Velocidad de la entidad
    float rotation;         // Rotación de la entidad
    float scale;            // Escala de la entidad
    Color color;            // Color para representar la entidad

public:
    // Constructor
    Entity(Vector2 pos = { 0, 0 }, Vector2 vel = { 0, 0 }, float rot = 0.0f, float scl = 1.0f, Color clr = WHITE)
        : position(pos), velocity(vel), rotation(rot), scale(scl), color(clr) {}

    // Destructor virtual para permitir la herencia
    virtual ~Entity() {}

    
    void SetPosition(Vector2 pos) { position = pos; }
    void SetVelocity(Vector2 vel) { velocity = vel; }
    void SetRotation(float rot) { rotation = rot; }
    void SetScale(float scl) { scale = scl; }
    void SetColor(Color clr) { color = clr; }

    
    Vector2 GetPosition() const { return position; }
    Vector2 GetVelocity() const { return velocity; }
    float GetRotation() const { return rotation; }
    float GetScale() const { return scale; }
    Color GetColor() const { return color; }

    // Métodos virtuales para herencia
    virtual void Update(float deltaTime) {
        // Actualizar posición basado en velocidad
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    }

    virtual void Draw() const {
        // Dibujar representación básica (puede ser sobreescrita por clases derivadas)
        DrawCircleV(position, 10.0f * scale, color);
    }
};

#endif // ENTITY_H
