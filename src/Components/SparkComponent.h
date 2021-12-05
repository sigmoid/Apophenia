#pragma once

#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "AttractableComponent.h"

#include <vector>
#include <string>
#include <glm/vec4.hpp>

struct SparkTrail
{
    std::vector<glm::vec2> Points;
    glm::vec4 StartColor, EndColor;
    int Width;
};

class SparkComponent : public Opal::Component
{
    public:
    SparkComponent(int width, glm::vec4 startColor, glm::vec4 endColor, int length, float updateTime);
    ~SparkComponent();
    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(std::shared_ptr<Opal::BatchRenderer2D> ctx) override;
    virtual void OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual std::shared_ptr<Opal::Component>Clone() override;

    void SetSpeedUp(float speedup);

    SparkTrail TrailData;

    private:
    std::shared_ptr<Opal::TransformComponent> mTransform;
    std::shared_ptr<Opal::VelocityComponent> mVelocity;

    float mSpeedUp = 1.0f;

    float mLineSpeed = 1000;
    float mTimer = 0, mDuration = 0;

    int mLength = 1;
};