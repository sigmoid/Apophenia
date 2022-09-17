#include "GameSettings.h"
#include "../Opal/Game.h"

bool GameSettings::AntiAliasingEnabled = false;
float GameSettings::MasterVolume = 1.0f;

void GameSettings::Load()
{
	AntiAliasingEnabled = Opal::PlayerPrefs::GetBool("AntiAliasingEnabled");
	MasterVolume = Opal::PlayerPrefs::GetFloat("MasterVolume");
	Opal::Game::Instance->mAudioEngine.SetGlobalVolume(MasterVolume);
}

bool GameSettings::GetAntiAliasingEnabled()
{
	return AntiAliasingEnabled;
}

void GameSettings::SetAntiAliasingEnabled(bool value)
{
	AntiAliasingEnabled = value;
	Opal::PlayerPrefs::SaveBool("AntiAliasingEnabled", value);
}

float GameSettings::GetMasterVolume()
{
	return MasterVolume;
}

void GameSettings::SetMasterVolume(float value)
{
	MasterVolume = value;
	Opal::PlayerPrefs::SaveFloat("MasterVolume", value);
	Opal::Game::Instance->mAudioEngine.SetGlobalVolume(MasterVolume);
}