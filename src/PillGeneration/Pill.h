#pragma once

#include "../../Opal/Game.h"
#include "../../Opal/OpalMath.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "../../Opal/Graphics/MeshRenderer2D.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>

class Pill
{
    public:
    Pill(glm::vec2 startPos, float startRot);

    void Update(float dTime);

    std::shared_ptr<Opal::Mesh2D> GetMesh();

    private:
    void GenerateEllipse();
    void GenerateCapsule();

    std::shared_ptr<Opal::Mesh2D> mMesh;
    std::vector<float> mVerts;

    glm::vec4 GetRandomCapsuleColor();

    glm::vec2 mPosition;
    float mSpeed = 0;
    float mAngularSpeed = 0;
    float mRotation;
};