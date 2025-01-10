#define _CRTDBG_MAP_ALLOC
#include "Application.h"
#include "Globals.h"

#include "raylib.h"

#include <stdlib.h>
#include <crtdbg.h> // Para detección de fugas de memoria

enum main_states
{
    MAIN_CREATION,
    MAIN_START,
    MAIN_UPDATE,
    MAIN_FINISH,
    MAIN_EXIT
};

int main(int argc, char** argv)
{
    // Configuración para detección de fugas de memoria
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    LOG("Starting game '%s'...", TITLE);
    SetTargetFPS(60);

    int main_return = EXIT_FAILURE;
    main_states state = MAIN_CREATION;
    Application* App = nullptr; // Inicializar como nullptr

    while (state != MAIN_EXIT)
    {
        switch (state)
        {
        case MAIN_CREATION:

            LOG("-------------- Application Creation --------------");
            App = new Application();
            state = MAIN_START;
            break;

        case MAIN_START:

            LOG("-------------- Application Init --------------");
            if (App->Init() == false)
            {
                LOG("Application Init exits with ERROR");
                state = MAIN_EXIT;
            }
            else
            {
                state = MAIN_UPDATE;
                LOG("-------------- Application Update --------------");
            }

            break;

        case MAIN_UPDATE:
        {
            int update_return = App->Update();

            if (update_return == UPDATE_ERROR)
            {
                LOG("Application Update exits with ERROR");
                state = MAIN_EXIT;
            }

            if (update_return == UPDATE_STOP)
                state = MAIN_FINISH;
        }
        break;

        case MAIN_FINISH:

            LOG("-------------- Application CleanUp --------------");
            if (App->CleanUp() == false)
            {
                LOG("Application CleanUp exits with ERROR");
            }
            else
                main_return = EXIT_SUCCESS;

            state = MAIN_EXIT;

            break;

        }
    }

    if (App != nullptr)
    {
        delete App;
        App = nullptr; // Limpia el puntero
    }

    LOG("Exiting game '%s'...\n", TITLE);

    // Reportar fugas de memoria
    _CrtDumpMemoryLeaks();

    return main_return;
}
