#include "DragComponent.h"

DragComponent::DragComponent(float amt)
{
    TypeName = "DragComponent";

    mFactor = amt;
}

DragComponent::~DragComponent()
{
    
}

void DragComponent::OnAdded()
{

}

void DragComponent::OnStart()
{
    mVelocity = mParent->GetComponent<Opal::VelocityComponent>();
}

void DragComponent::Update(float dTime)
{
    glm::vec3 vel = mVelocity->GetVelocity();
    vel.x = Opal::OpalMath::Approach(vel.x, 0, dTime);
    vel.y = Opal::OpalMath::Approach(vel.y, 0, dTime);
    vel.z = Opal::OpalMath::Approach(vel.z, 0, dTime);
    mVelocity->SetVelocity(vel);
}

void DragComponent::Render(std::shared_ptr<Opal::BatchRenderer2D>   ctx)
{

}

void DragComponent::OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void DragComponent::Serialize()
{

}

void DragComponent::Deserialize()
{

}

std::shared_ptr<Opal::Component> DragComponent::Clone()
{
    return std::make_shared<DragComponent>(1);
}
