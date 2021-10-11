#pragma once

#include <string>

struct SentenceFragment
{
    std::string Text;
    float Attraction;
    bool IsIntrusive = false;
};