#include "CursorComponent.h"
#include <iostream>
#include "../../Opal/Input/InputHandler.h"
#include "../../Opal/OpalMath.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/EntityComponent/Scene.h"
#include "SentenceFragmentComponent.h"
#include "EndWallComponent.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../../Opal/vendor/imgui/imgui.h"
#include "../../Opal/vendor/imgui/imgui_impl_glfw.h"
#include "../../Opal/vendor/imgui/imgui_impl_vulkan.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "AttractableComponent.h"

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
    mAttractable = mParent->GetComponent<AttractableComponent>();

    mMesh = Opal::Game::Instance->Renderer->CreateMesh((mNumTris+1) * 3);
}

void CursorComponent::Reset()
{
    mAlive = true;
    mCurrentResponse.clear();
}

void CursorComponent::Update(float dTime) 
{
    mTimeSinceBirth += dTime;
    std::vector<Opal::Entity *> ents = Opal::Scene::GetActiveScene()->GetAllEntities();


    mAttractors = mAttractable->GetFrameAttractors();

    UpdateVerts();

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

    if(!takingInput)
        mCurrentSpeed = 0;

    auto vel = mParent->GetComponent<Opal::VelocityComponent>();
    vel->SetVelocity(vel->GetVelocity() + glm::vec3(0,mCurrentSpeed, 0));

    mLastSpeed = mCurrentSpeed;
}

void CursorComponent::Render(Opal::BatchRenderer2D *ctx) 
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGui::Begin("Player Info");

    ImGui::DragFloat("Current Speed", &mCurrentSpeed);
    int na = mAttractors.size();
    ImGui::InputInt("Num Attractors", &na);

    ImGui::DragFloat("Max Speed", &mMaxSpeed);
    ImGui::DragFloat("Acceleration", &mAcceleration);

    ImGui::End();
    ImGui::Render();
    ImGui::EndFrame();
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

Opal::Mesh2D *CursorComponent::GetMesh()
{
    return mMesh;
}

void CursorComponent::UpdateVerts()
{
    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplGlfw_NewFrame();

    // ImGui::NewFrame();

    // ImGui::Begin("Noise settings");

    // ImGui::DragFloat("Radius", &mRadius);
    // ImGui::DragFloat("NoiseFactor", &mNoiseIntensity);
    // ImGui::DragFloat("Frequency", &mNoiseFreq, 0.01);
    // ImGui::DragFloat("NoiseScale", &mNoiseScale, 0.1);
    // ImGui::DragFloat("Attraction", &mAttractionModifier, 0.1);
    // ImGui::DragInt("Falloff", &mAttractionFalloff);
    // ImGui::DragFloat("AttractionScale", &mAttractionScale);

    // ImGui::End();
    // ImGui::Render();
    // ImGui::EndFrame();

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(mNoiseFreq);
    mVertices.clear();
    
    glm::vec2 startPos = glm::vec2(mTransform->Position.x, mTransform->Position.y);   
    glm::vec2 lastPos = startPos;
    glm::vec2 firstPos = lastPos;
    float firstRadius = 0;
    float lastRadius = 0;

    float noiseSeed = mTimeSinceBirth;
    bool first = true;

    for(int i = 0; i < mNumTris; i++)
    {
        float noiseVal = noise.GetNoise<float>((i * mNoiseScale),noiseSeed) * mNoiseIntensity;
        glm::vec2 curPos = startPos;

        float theta = ((float)i / (float)mNumTris) * 2.0f * 3.14159f;

        float attraction = 0.0f;

        for(int j = 0; j < mAttractors.size(); j++)
        {
            glm::vec2 attractorDist = mAttractors[j].Position - startPos;
            glm::vec2 attractorVec = glm::normalize(attractorDist);
            glm::vec2 thisVec = glm::normalize(glm::vec2(cos(theta), sin(theta)));

            float dotprod = glm::dot(thisVec, attractorVec);
            float dotprodRaised = pow(dotprod,mAttractionFalloff);
            if(dotprod > 0)
            {
                attraction += dotprodRaised * mAttractionModifier * (mAttractors[j].Strength)/mAttractionScale * ((glm::length(attractorDist)<100)? (glm::length(attractorDist)/100.0f):1.0f);
            }            
        }

        float newRadius = mRadius + noiseVal + attraction;

        float circleProgress = (float)i / (float)mNumTris; 
        if(circleProgress > 0.8)
        {
            newRadius = Opal::OpalMath::Approach(mRadius + noiseVal + attraction, firstRadius, (circleProgress - 0.8f) * 5.0f);
        }

        curPos.x += cos(theta) * newRadius;
        curPos.y += sin(theta) * newRadius;

        mVertices.push_back(startPos.x);
        mVertices.push_back(startPos.y);
        mVertices.push_back(mMeshColor.r);
        mVertices.push_back(mMeshColor.g);
        mVertices.push_back(mMeshColor.b);
        mVertices.push_back(mMeshColor.a);
        mVertices.push_back(lastPos.x);
        mVertices.push_back(lastPos.y);
        mVertices.push_back(mMeshColor.r);
        mVertices.push_back(mMeshColor.g);
        mVertices.push_back(mMeshColor.b);
        mVertices.push_back(mMeshColor.a);
        mVertices.push_back(curPos.x);
        mVertices.push_back(curPos.y);
        mVertices.push_back(mMeshColor.r);
        mVertices.push_back(mMeshColor.g);
        mVertices.push_back(mMeshColor.b);
        mVertices.push_back(mMeshColor.a);

        if(first)
        {
            firstPos = curPos;
            firstRadius = newRadius;
            first = false;
        }
        else if(i == mNumTris - 1) //last
        {
            mVertices.push_back(startPos.x);
            mVertices.push_back(startPos.y);
            mVertices.push_back(mMeshColor.r);
            mVertices.push_back(mMeshColor.g);
            mVertices.push_back(mMeshColor.b);
            mVertices.push_back(mMeshColor.a);
            mVertices.push_back(curPos.x);
            mVertices.push_back(curPos.y);
            mVertices.push_back(mMeshColor.r);
            mVertices.push_back(mMeshColor.g);
            mVertices.push_back(mMeshColor.b);
            mVertices.push_back(mMeshColor.a);
            mVertices.push_back(firstPos.x);
            mVertices.push_back(firstPos.y);
            mVertices.push_back(mMeshColor.r);
            mVertices.push_back(mMeshColor.g);
            mVertices.push_back(mMeshColor.b);
            mVertices.push_back(mMeshColor.a);
        }
        lastPos = curPos;
        lastRadius = newRadius;
    }
    mMesh->SetVertices(mVertices.data(), mVertices.size() * sizeof(float));
}