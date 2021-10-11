#pragma once

#include <string>

#include <glm/vec4.hpp>

#include "Response.h"

struct Prompt
{
    std::string Text;
    glm::vec4 Color;
    std::vector<std::string> Triggers;
    Response Response;
    bool IsEnd = false;
    bool IsKill = true;

    std::string TransitionText = "";
};