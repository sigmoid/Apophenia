#include "CursorComponent.h"
#include <iostream>
#include "../../Opal/Input/InputHandler.h"
#include "../../Opal/OpalMath.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/EntityComponent/Scene.h"
#include "SentenceFragmentComponent.h"
#include "EndWallComponent.h"

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
    mCurrentSpeed = 0;
}

void CursorComponent::Reset()
{
    mAlive = true;
    mCurrentResponse.clear();
}

void CursorComponent::Update(float dTime) 
{
    std::vector<Opal::Entity *> ents = Opal::Scene::GetActiveScene()->GetAllEntities();

    for(int i = 0; i < ents.size(); i++)
    {
        SentenceFragmentComponent *otherSentence = ents[i]->GetComponent<SentenceFragmentComponent>();

        if(otherSentence != nullptr && otherSentence->Attraction != 0)
        {
            Opal::TransformComponent *otherTrans = ents[i]->GetComponent<Opal::TransformComponent>();
            if(otherTrans == nullptr)
            {
                std::cout << "No transform on sentence fragment?!" << std::endl;
            }
            else
            {
                if(0 < (otherTrans->Position.x - mTransform->Position.x) && (otherTrans->Position.x - mTransform->Position.x) < 1000 && abs(mTransform->Position.y - otherTrans->Position.y) > 50)
                {
                    mCurrentSpeed += otherSentence->Attraction * dTime * (((otherTrans->Position.y - mTransform->Position.y) < 0) ? -1 : 1);
                }
            }
        }
    }

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

    mParent->GetComponent<Opal::VelocityComponent>()->SetVelocity(glm::vec3(0,mCurrentSpeed, 0));
}

void CursorComponent::Render(Opal::BatchRenderer2D *ctx) 
{

}

bool CursorComponent::GetAlive()
{
    return mAlive;
}

std::vector<std::string> CursorComponent::GetResponse()
{
    return mCurrentResponse;
}

void CursorComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB) 
{
    if(other->GetComponent<EndWallComponent>() != nullptr)
    {
        mAlive = false;
    }

    SentenceFragmentComponent *otherComp = other->GetComponent<SentenceFragmentComponent>(); 
    if(otherComp != nullptr && otherComp->GetActive())
    {
        mCurrentResponse.push_back(other->GetComponent<SentenceFragmentComponent>()->Text);
        otherComp->Interact();
    }
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
