#include "CursorComponent.h"
#include <iostream>
#include "../../Opal/Input/InputHandler.h"
#include "../../Opal/OpalMath.h"

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
    bool takingInput = false;

    if(Opal::InputHandler::GetKey(mUpBinding))
    {
        takingInput = !takingInput;
        mCurrentSpeed -= mAcceleration * dTime;
    }
    if(Opal::InputHandler::GetKey(mDownBinding))
    {
        takingInput = !takingInput;
        mCurrentSpeed += mAcceleration * dTime;
    }

    if(mCurrentSpeed > mMaxSpeed)
    {
        mCurrentSpeed = mMaxSpeed;
    }
    if(mCurrentSpeed < -mMaxSpeed)
    {
        mCurrentSpeed = -mMaxSpeed;
    }

    if(!takingInput && (mCurrentSpeed > dTime * mAcceleration || mCurrentSpeed < -dTime * mAcceleration))
    {
        mCurrentSpeed = Opal::OpalMath::Approach(mCurrentSpeed, 0, dTime);
    }
    else if(!takingInput)
    {
        mCurrentSpeed = 0;
    }

    mTransform->Position.y += mCurrentSpeed * dTime;
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
