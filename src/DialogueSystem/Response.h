#pragma once 

#include "SentenceFragment.h"

#include <string>
#include <glm/vec4.hpp>
#include <vector>

struct Response
{
    std::vector<std::vector<SentenceFragment> > Fragments;
    float Speed;
    glm::vec4 Color;
    std::string WordBank;
    float WordFrequency;
    bool SolidWords;
    float ChannelSize = 500;
    bool AllowNonsense = false;
    bool RequireCoreOnly = false;
    bool Drunk = false;
    bool Tremor = false;
    bool ShowBlur = false;
};