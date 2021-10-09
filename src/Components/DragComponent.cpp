#include "DragComponent.h"

DragComponent::DragComponent(float amt)
{
    TypeName = "DragComponent";

    mFactor = amt;
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

void DragComponent::Render(Opal::BatchRenderer2D *ctx)
{

}

void DragComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void DragComponent::Serialize()
{

}

void DragComponent::Deserialize()
{

}

Opal::Component *DragComponent::Clone()
{
    return new DragComponent(1);
}
