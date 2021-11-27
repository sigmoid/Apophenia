#pragma once

#include <string>

#include <glm/vec4.hpp>

#include "Response.h"
#include <vector>

struct Prompt
{
    std::string Text;
    std::vector<glm::vec4> Colors;
    std::vector<std::string> Triggers;
    std::vector<std::string> ObscuredWords;
    Response Response;
    bool IsEnd = false;
    bool IsKill = true;
    int Sequence=-1;

    std::string TransitionText = "";
};