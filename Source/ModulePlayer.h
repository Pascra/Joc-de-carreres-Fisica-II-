// ModulePlayer.h
#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	~ModulePlayer();

	bool Start() override;
	update_status Update() override;
	bool CleanUp() override;

private:
	Texture2D car_texture;     // Textura del coche
	Vector2 car_position;      // Posici�n del coche
	float car_rotation;        // Rotaci�n del coche
	float speed;               // Velocidad actual del coche
	float acceleration;        // Aceleraci�n del coche
	float max_speed;           // Velocidad m�xima del coche
	float handling;            // Manejo del coche (agilidad al girar)
};
