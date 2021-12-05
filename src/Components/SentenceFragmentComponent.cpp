#include "SentenceFragmentComponent.h"
#include "../../Opal/EntityComponent/Scene.h"

SentenceFragmentComponent::SentenceFragmentComponent()
{
    TypeName = "SentenceFragmentComponent";
}

SentenceFragmentComponent::SentenceFragmentComponent(std::string text, float speed, glm::vec4 color, float attraction, bool intrusive, bool solid, bool core)
{
    TypeName = "SentenceFragmentComponent";
    Text = text;
    Speed = speed;
    Color = color;
    Attraction = attraction;
    IsIntrusive = intrusive;
    mSolid = solid;
    mCore = core;

    if(IsIntrusive)
        Color = IntrusiveColor;

    if(!mSolid)
        Color.a = 0.125f;
}

SentenceFragmentComponent::~SentenceFragmentComponent()
{
    
}

bool SentenceFragmentComponent::GetSolid()
{
    return mSolid;
}

void SentenceFragmentComponent::OnAdded()
{

}

void SentenceFragmentComponent::OnStart()
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
}

void SentenceFragmentComponent::Update(float dTime)
{
    mTransform->Position.x -= Speed * dTime;
}

bool SentenceFragmentComponent::GetCore()
{
    return mCore;    
}

void SentenceFragmentComponent::Interact()
{
    mActive = false;
    Color.a = 0.2f;
}

bool SentenceFragmentComponent::GetActive()
{
    return mActive;
}

void SentenceFragmentComponent::Render(std::shared_ptr<Opal::BatchRenderer2D>   ctx)
{

}

void SentenceFragmentComponent::OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB)
{
}

void SentenceFragmentComponent::Serialize()
{

}

void SentenceFragmentComponent::Deserialize()
{

}

std::shared_ptr<Opal::Component> SentenceFragmentComponent::Clone()
{
    return std::make_shared<SentenceFragmentComponent>();
}
