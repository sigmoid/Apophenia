#pragma once

#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/EntityComponent/TransformComponent.h"

#include <string>
#include <glm/vec4.hpp>

class ScrollingWordComponent : public Opal::Component
{
    public:
    ScrollingWordComponent();
    ScrollingWordComponent(std::string text, float speed, glm::vec4 color);

    virtual void OnAdded() override;
    virtual void OnStart() override;
    virtual void Update(float dTime) override;
    virtual void Render(Opal::BatchRenderer2D *ctx) override;
    virtual void OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB) override;
    virtual void Serialize() override;
    virtual void Deserialize() override;
    virtual Opal::Component *Clone() override;

    std::string Text;
    float Speed = 100;
    glm::vec4 Color = glm::vec4(0.9,0.9,0.9,1.0);
    glm::vec4 IntrusiveColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    private:
    Opal::TransformComponent *mTransform;
    bool mActive = true;
};