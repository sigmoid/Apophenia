#include "EndWallComponent.h"

EndWallComponent::EndWallComponent()
{
    TypeName = "EndWallComponent";
}

EndWallComponent::EndWallComponent(float speed)
{
    TypeName = "EndWallComponent";
    mSpeed = speed;
}

void EndWallComponent::OnAdded()
{

}

void EndWallComponent::OnStart()
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
}

void EndWallComponent::Update(float dTime)
{
    mTransform->Position.x -= mSpeed * dTime;
}

void EndWallComponent::Render(Opal::BatchRenderer2D *ctx)
{

}

void EndWallComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void EndWallComponent::Serialize()
{

}

void EndWallComponent::Deserialize()
{

}

Opal::Component *EndWallComponent::Clone()
{
    return new EndWallComponent();
}
