#pragma once

#include "../Opal/Audio/AudioClip.h"
#include <string>
#include <map>
#include <memory>

class AudioBank
{
public:
	AudioBank();
	static std::shared_ptr<AudioBank> Instance;

	void LoadClip(std::string name);
	std::shared_ptr<Opal::AudioClip> GetClip(std::string name);

private:
	std::map<std::string, std::shared_ptr<Opal::AudioClip> > mClips;
};