#include "CursorComponent.h"
#include <iostream>
#include "../../Opal/Input/InputHandler.h"

CursorComponent::CursorComponent()
{
    TypeName = "CursorComponent";
}

void CursorComponent::OnAdded() 
{

}

void CursorComponent::OnStart() 
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
}

void CursorComponent::Update(float dTime) 
{
    if(Opal::InputHandler::GetKey(mUpBinding))
    {
        mTransform->Position.y -= mSpeed * dTime;
    }
    if(Opal::InputHandler::GetKey(mDownBinding))
    {
        mTransform->Position.y += mSpeed * dTime;
    }
}

void CursorComponent::Render(Opal::BatchRenderer2D *ctx) 
{

}

void CursorComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB) 
{
    std::cout << "Cursor hit something!" << std::endl;
}

void CursorComponent::Serialize() 
{

}

void CursorComponent::Deserialize() 
{

}

Opal::Component *CursorComponent::Clone() 
{
    return new CursorComponent();
}
