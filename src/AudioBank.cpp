#include "AudioBank.h"
#include "../Opal/Game.h"
#include "../Opal/util.h"

std::shared_ptr<AudioBank> AudioBank::Instance = std::make_shared<AudioBank>();

AudioBank::AudioBank()
{
}

void AudioBank::LoadClip(std::string name)
{
	if (mClips.find(name) == mClips.end())
	{
		std::cout << Opal::GetBaseContentPath().append("Audio/").append(name) << std::endl;
		auto clip = Opal::Game::Instance->mAudioEngine.LoadClip(Opal::GetBaseContentPath().append("Audio/").append(name));
		mClips[name] = clip;
	}
}

std::shared_ptr<Opal::AudioClip> AudioBank::GetClip(std::string name)
{
	if (mClips.find(name) != mClips.end())
	{
		return mClips[name];
	}
	return nullptr;
}
