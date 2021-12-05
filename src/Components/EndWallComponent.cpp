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

EndWallComponent::~EndWallComponent()
{
    
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

void EndWallComponent::Render(std::shared_ptr<Opal::BatchRenderer2D>   ctx)
{

}

void EndWallComponent::OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void EndWallComponent::Serialize()
{

}

void EndWallComponent::Deserialize()
{

}

std::shared_ptr<Opal::Component> EndWallComponent::Clone()
{
    return std::make_shared<EndWallComponent>();
}
