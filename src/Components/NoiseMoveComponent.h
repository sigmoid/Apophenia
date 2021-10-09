#pragma once

#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/vendor/FastNoiseLite.h"


#include <vector>
#include <string>
#include <glm/vec4.hpp>

class NoiseMoveComponent : public Opal::Component
{
    public:
    NoiseMoveComponent(float strength, float freq, float scale, float seed);
    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(Opal::BatchRenderer2D *ctx) override;
    virtual void OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual Opal::Component *Clone() override;

    private:
    float mStrength = 1, mNoiseScale = 1, mFrequency = 1, mSeed;
    float mTimeSinceBirth = 0;

    Opal::VelocityComponent *mVelocity = nullptr;
    FastNoiseLite mNoise;

};