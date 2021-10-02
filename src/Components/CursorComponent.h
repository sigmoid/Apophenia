#pragma once

#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"

class CursorComponent : public Opal::Component
{
    public:
    CursorComponent();
    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(Opal::BatchRenderer2D *ctx) override;
    virtual void OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual Opal::Component *Clone() override;

    private:
    float mSpeed = 200;
    int mUpBinding = GLFW_KEY_W, mDownBinding = GLFW_KEY_S;
    Opal::TransformComponent *mTransform;
};