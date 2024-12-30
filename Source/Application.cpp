// Application.cpp
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"
#include "ModulePlayer.h"
#include "ModuleItem.h"
#include "Application.h"

Application::Application()
{
    window = new ModuleWindow(this);
    renderer = new ModuleRender(this);
    audio = new ModuleAudio(this, true);
    physics = new ModulePhysics(this);
    scene_intro = new ModuleGame(this);
    player = new ModulePlayer(this);
    item = new ModuleItem(this);
    game = new ModuleGame(this, true);

    // The order of calls is very important!
    // Modules will Init() Start() and Update in this order
    // They will CleanUp() in reverse order

    // Main Modules
    AddModule(window);
    AddModule(physics);
    AddModule(audio);

    // Scene Modules
    AddModule(scene_intro); // Fondo
    AddModule(player);      // Coche
    AddModule(item);

    // Rendering happens at the end
    AddModule(renderer);
}

Application::~Application()
{
    TraceLog(LOG_INFO, "Shutting down application...");

    // Limpiar todos los módulos en orden inverso
    for (auto it = list_modules.rbegin(); it != list_modules.rend(); ++it)
    {
        Module* item = *it;
        TraceLog(LOG_INFO, "Deleting module: %p", item);
        delete item; // Elimina cada módulo
        *it = nullptr; // Establece el puntero a nullptr (opcional, si no se va a usar después)
    }

    // Si `game` no está en la lista de módulos, elimínalo por separado
    if (game)
    {
        TraceLog(LOG_INFO, "Deleting game module.");
        delete game;
        game = nullptr; // Limpia el puntero de `game`
    }

    // Limpiar la lista
    list_modules.clear();
    TraceLog(LOG_INFO, "Application shutdown complete.");
}


bool Application::Init()
{
    bool ret = true;

    // Call Init() in all modules
    for (auto it = list_modules.begin(); it != list_modules.end() && ret; ++it)
    {
        Module* module = *it;
        ret = module->Init();
    }

    // After all Init calls we call Start() in all modules
    LOG("Application Start --------------");

    for (auto it = list_modules.begin(); it != list_modules.end() && ret; ++it)
    {
        Module* module = *it;
        ret = module->Start();
    }

    return ret;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
    update_status ret = UPDATE_CONTINUE;

    for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
    {
        Module* module = *it;
        if (module->IsEnabled())
        {
            ret = module->PreUpdate();
        }
    }

    for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
    {
        Module* module = *it;
        if (module->IsEnabled())
        {
            ret = module->Update();
        }
    }

    for (auto it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; ++it)
    {
        Module* module = *it;
        if (module->IsEnabled())
        {
            ret = module->PostUpdate();
        }
    }

    if (WindowShouldClose()) ret = UPDATE_STOP;

    return ret;
}

bool Application::CleanUp()
{
    bool ret = true;
    for (auto it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
    {
        Module* item = *it;
        ret = item->CleanUp();
    }

    return ret;
}

void Application::AddModule(Module* mod)
{
    list_modules.emplace_back(mod);
}
