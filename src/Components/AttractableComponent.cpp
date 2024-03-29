#include "AttractableComponent.h"
#include "SentenceFragmentComponent.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "../../Opal/EntityComponent/Scene.h"

AttractableComponent::AttractableComponent(bool lock_x)
{
    TypeName = "AttractableComponent";

    mLocked = lock_x;
}

AttractableComponent::~AttractableComponent()
{
    
}

void AttractableComponent::OnAdded() 
{

}

void AttractableComponent::OnStart() 
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
    mVelocity = mParent->GetComponent<Opal::VelocityComponent>();
}

void AttractableComponent::SetPlayer(bool isPlayer)
{
    mIsPlayer = isPlayer;
}

void AttractableComponent::Update(float dTime) 
{
    mFrameAttractors.clear();

    std::vector<std::shared_ptr<Opal::Entity> > ents = Opal::Scene::GetActiveScene()->GetAllEntities();
    for (int i = 0; i < ents.size(); i++)
    {
        std::shared_ptr<SentenceFragmentComponent> otherSentence = ents[i]->GetComponent<SentenceFragmentComponent>();

        if (otherSentence != nullptr && otherSentence->Attraction != 0 && (!mIsPlayer || mIsPlayer && !otherSentence->GetIgnorePlayer()))
        {
            std::shared_ptr<Opal::BoxColliderComponent2D> otherBox = ents[i]->GetComponent<Opal::BoxColliderComponent2D>();
            if(otherBox == nullptr)
            {
                std::cout << "No BoxCollider on sentence fragment?!" << std::endl;
            }
            else
            {
                float dist = glm::distance(otherBox->GetAABB().GetCenter(), glm::vec2(mTransform->Position));
                if(-200 < (otherBox->GetAABB().GetCenter().x - mTransform->Position.x) && (otherBox->GetAABB().GetCenter().x - mTransform->Position.x) < mAttractionScale)
                {
                    Attractor thisAttr;
                    thisAttr.Position = glm::vec2(otherBox->GetAABB().GetCenter().x, otherBox->GetAABB().GetCenter().y);

                    if(mLocked)
                    {
                        if(dist > 100)
                            mVelocity->SetVelocity( mVelocity->GetVelocity() + glm::vec3(0,
                            (250 * (otherSentence->Attraction / dist) * dTime * (((otherBox->GetAABB().GetCenter().y - mTransform->Position.y) < 0) ? -1 : 1)),
                            0));
                    }
                    else
                    {
                        mVelocity->SetVelocity( mVelocity->GetVelocity() +
                        glm::normalize(glm::vec3(otherBox->GetAABB().GetCenter(), 0) - mTransform->Position) * (250 * (otherSentence->Attraction / dist) * dTime)
                        );
                    }

                    thisAttr.Strength = 250 * (otherSentence->Attraction / dist);

                    mFrameAttractors.push_back(thisAttr);
                }
            }
        }
    }
}

void AttractableComponent::Render(std::shared_ptr<Opal::BatchRenderer2D> ctx) 
{

}

void AttractableComponent::OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB) 
{

}

void AttractableComponent::Serialize() 
{

}

void AttractableComponent::Deserialize() 
{

}

std::shared_ptr<Opal::Component> AttractableComponent::Clone() 
{
    return std::make_shared<AttractableComponent>(true);
}

std::vector<Attractor> AttractableComponent::GetFrameAttractors()
{
    return mFrameAttractors;
}