#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"

#include "raylib.h"

#define MAX_FX_SOUNDS   64

ModuleAudio::ModuleAudio(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	fx_count = 0;
	music = Music{ 0 };
}

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init()
{
	LOG("Loading Audio Mixer");

	InitAudioDevice();
	if (!IsAudioDeviceReady())
	{
		LOG("Error: Audio device could not be initialized");
		return false;
	}

	return true;
}


// Called before quitting
bool ModuleAudio::CleanUp()
{
	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	// Unload sounds
	for (unsigned int i = 0; i < fx_count; i++)
	{
		if (fx[i].stream.buffer != nullptr)
		{
			UnloadSound(fx[i]);
			fx[i] = Sound{ 0 }; // Restablece para evitar accesos no v�lidos
		}
	}

	fx_count = 0; // Reinicia el contador de sonidos

	// Unload music
	if (IsMusicReady(music))
	{
		StopMusicStream(music);
		UnloadMusicStream(music);
		music = Music{ 0 }; // Restablece para evitar problemas
	}

	CloseAudioDevice();

	return true;
}

// Play a music file
bool ModuleAudio::PlayMusic(const char* path, float fade_time)
{
	if(IsEnabled() == false)
		return false;

	bool ret = true;
	
    StopMusicStream(music);
    music = LoadMusicStream(path);
    
    PlayMusicStream(music);

	LOG("Successfully playing %s", path);

	return ret;
}

// Load WAV
unsigned int ModuleAudio::LoadFx(const char* path)
{
	if(IsEnabled() == false)
		return 0;

	unsigned int ret = 0;

	Sound sound = LoadSound(path);

	if(sound.stream.buffer == NULL)
	{
		LOG("Cannot load sound: %s", path);
	}
	else
	{
        fx[fx_count] = sound;
		ret = fx_count++;
	}

	return ret;
}

// Play WAV
bool ModuleAudio::PlayFx(unsigned int id, int repeat)
{
	if (IsEnabled() == false)
	{
		return false;
	}

	bool ret = false;

	if(id < fx_count) PlaySound(fx[id]);

	return ret;
}