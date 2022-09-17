#pragma once

#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"

#include <vector>
#include <string>
#include <glm/vec4.hpp>

struct Attractor 
{
    float Strength;
    glm::vec2 Position;
};

class AttractableComponent : public Opal::Component
{
    public:
    AttractableComponent(bool lock_x);
    ~AttractableComponent();

    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(std::shared_ptr<Opal::BatchRenderer2D> ctx) override;
    virtual void OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual std::shared_ptr<Component> Clone() override;

    void SetPlayer(bool isPlayer);

    std::vector<Attractor> GetFrameAttractors();

    private:
    std::shared_ptr<Opal::TransformComponent> mTransform = nullptr;
    std::shared_ptr<Opal::VelocityComponent> mVelocity = nullptr; 

    float mAttractionScale = 1200.0f;
    std::vector<Attractor> mFrameAttractors;
    bool mLocked = true;
    bool mIsPlayer = false;
};