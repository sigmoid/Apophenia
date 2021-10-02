#include "SentenceFragmentComponent.h"

SentenceFragmentComponent::SentenceFragmentComponent()
{
    TypeName = "SentenceFragmentComponent";
}

SentenceFragmentComponent::SentenceFragmentComponent(std::string text, float speed, glm::vec4 color)
{
    TypeName = "SentenceFragmentComponent";
    Text = text;
    Speed = speed;
    Color = color;
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

void SentenceFragmentComponent::Interact()
{
    mActive = false;
    Color = glm::vec4(1, 0, 0 , 1);
}

bool SentenceFragmentComponent::GetActive()
{
    return mActive;
}

void SentenceFragmentComponent::Render(Opal::BatchRenderer2D *ctx)
{

}

void SentenceFragmentComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void SentenceFragmentComponent::Serialize()
{

}

void SentenceFragmentComponent::Deserialize()
{

}

Opal::Component *SentenceFragmentComponent::Clone()
{
    return new SentenceFragmentComponent();
}
