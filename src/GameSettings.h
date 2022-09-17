#pragma once

#include <memory>
#include "PlayerPrefs.h"

class GameSettings
{
public:
	static bool GetAntiAliasingEnabled();
	static float GetMasterVolume();

	static void Load();

	static void SetAntiAliasingEnabled(bool value);
	static void SetMasterVolume(float value);

private:
	static bool AntiAliasingEnabled;
	static float MasterVolume;
};