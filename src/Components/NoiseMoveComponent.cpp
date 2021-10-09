#include "NoiseMoveComponent.h"

NoiseMoveComponent::NoiseMoveComponent(float strength, float freq, float scale, float seed)
{
    mStrength = strength;
    mNoise.SetFrequency(freq);
    mFrequency = freq;
    mNoiseScale = scale;
    mSeed = seed;
}

void NoiseMoveComponent::OnAdded()
{

}

void NoiseMoveComponent::OnStart()
{
    mVelocity = mParent->GetComponent<Opal::VelocityComponent>();
}

void NoiseMoveComponent::Update(float dTime)
{
    mTimeSinceBirth += dTime;

    float yVel = mNoise.GetNoise((mTimeSinceBirth) * mNoiseScale, mSeed) * mStrength;
    mVelocity->SetVelocity( mVelocity->GetVelocity() + glm::vec3(0,yVel,0));
}

void NoiseMoveComponent::Render(Opal::BatchRenderer2D *ctx)
{

}

void NoiseMoveComponent::OnCollision(Opal::Entity *other, glm::vec2 resolution, Opal::AABB otherAABB)
{

}

void NoiseMoveComponent::Serialize()
{

}

void NoiseMoveComponent::Deserialize()
{

}

Opal::Component *NoiseMoveComponent::Clone()
{
    return new NoiseMoveComponent(mStrength, mFrequency, mNoiseScale, mSeed);
}
