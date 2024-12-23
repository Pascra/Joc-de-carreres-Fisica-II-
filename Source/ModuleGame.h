#pragma once

#include "Module.h"
#include "raylib.h"
#include "Utils.h"
#include "ModulePhysics.h"

class ModuleGame : public Module
{
public:
    ModuleGame(Application* app, bool start_enabled = true);
    ~ModuleGame();

    bool Start() override;
    update_status Update() override;
    bool CleanUp() override;

private:
    Texture2D map_texture; // Textura del mapa
};
