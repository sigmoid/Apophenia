#include "ScrollingWordComponent.h"

ScrollingWordComponent::ScrollingWordComponent()
{
    TypeName = "ScrollingWordComponent";
}

ScrollingWordComponent::ScrollingWordComponent(std::string text, float speed, glm::vec4 color)
{
    TypeName = "ScrollingWordComponent";
    Text = text;
    Speed = speed;
    Color = color;
}

void ScrollingWordComponent::OnAdded()
{

}

void ScrollingWordComponent::OnStart()
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
}

void ScrollingWordComponent::Update(float dTime)
{
    mTransform->Position.x -= Speed * dTime;
}

void ScrollingWordComponent::Render(Opal::BatchRenderer2D *ctx)
{

}

void ScrollingWordComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void ScrollingWordComponent::Serialize()
{

}

void ScrollingWordComponent::Deserialize()
{

}

Opal::Component *ScrollingWordComponent::Clone()
{
    return new ScrollingWordComponent();
}
