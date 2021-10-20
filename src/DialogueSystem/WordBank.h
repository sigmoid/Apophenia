#pragma once
#include <string>
#include <glm/vec4.hpp>


struct Word
{
    std::string Text;
    float Probability = 1;
    float Speed = 1;
    glm::vec4 Color = glm::vec4(1,1,1,0.0125f);
};
