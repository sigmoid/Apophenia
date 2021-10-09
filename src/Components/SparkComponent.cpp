#include "SparkComponent.h"

SparkComponent::SparkComponent(int width, glm::vec4 startColor, glm::vec4 endColor, int length, float updateTime)
{
    TypeName = "SparkComponent";

    TrailData.Width = width;
    TrailData.StartColor = startColor;
    TrailData.EndColor = endColor;
    mLength = length;
    mDuration = updateTime;
}

void SparkComponent::OnAdded()
{

}

void SparkComponent::OnStart()
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
    mVelocity = mParent->GetComponent<Opal::VelocityComponent>();
    mLineSpeed = mVelocity->GetVelocity().x;
}

void SparkComponent::Update(float dTime)
{
    mTimer -= dTime;

    if(mTimer <= 0)
    {
        TrailData.Points.push_back(glm::vec2(mTransform->Position));

        if(TrailData.Points.size() > mLength)
        {
            TrailData.Points.erase(TrailData.Points.begin(), TrailData.Points.begin() + 1);
        }

        mTimer = mDuration;
    }

    mVelocity->SetVelocity(mVelocity->GetVelocity() * glm::vec3(0,1,1));
    mTransform->Position += mVelocity->GetVelocity() * dTime;

    for(int i = 0; i < TrailData.Points.size(); i++)
    {
        TrailData.Points[i] -= glm::vec2(mLineSpeed * dTime, 0);
    }
}

void SparkComponent::Render(Opal::BatchRenderer2D *ctx)
{

}

void SparkComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void SparkComponent::Serialize()
{

}

void SparkComponent::Deserialize()
{

}

Opal::Component *SparkComponent::Clone()
{
    return new SparkComponent(1,glm::vec4(1),glm::vec4(1), 5, 0.01f);
}
