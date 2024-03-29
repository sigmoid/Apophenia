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
#include "../../Opal/vendor/imgui/imgui_impl_sdl.h"
#include "../../Opal/vendor/imgui/imgui_impl_vulkan.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "AttractableComponent.h"
#include "../AudioBank.h"

CursorComponent::CursorComponent()
{
    TypeName = "CursorComponent";
}

CursorComponent::~CursorComponent()
{
}

void CursorComponent::OnAdded() 
{

}

void CursorComponent::KillAmbience()
{
    if (mAmbienceInstance != nullptr)
    {
        mAmbienceInstance->Stop();
    }

}

void CursorComponent::StartAmbience()
{
    auto ambienceClip = AudioBank::Instance->GetClip("ambience.wav");
    mAmbienceInstance = Opal::Game::Instance->mAudioEngine.PlaySound(ambienceClip, 0.125f, 1.0f, 0.0f, true, true);
}
void CursorComponent::OnStart() 
{
    mTransform = mParent->GetComponent<Opal::TransformComponent>();
    mCurrentSpeed = 0;
    mAttractable = mParent->GetComponent<AttractableComponent>();

    mMesh = Opal::Game::Instance->Renderer->CreateMesh((mNumTris+1) * 3);
    mLastRadii.resize(mNumTris, -1);

    // Only loads the first time.
    AudioBank::Instance->LoadClip("snap1.wav");
    AudioBank::Instance->LoadClip("snap2.wav");
    AudioBank::Instance->LoadClip("snap3.wav");

    AudioBank::Instance->LoadClip("blow1.wav");
    AudioBank::Instance->LoadClip("blow2.wav");
    AudioBank::Instance->LoadClip("blow3.wav");
    AudioBank::Instance->LoadClip("blow4.wav");

    AudioBank::Instance->LoadClip("ambience.wav");    
}

bool CursorComponent::ShouldPop(bool reset)
{
    bool res = mShouldPop;
    mShouldPop = reset;
    return res;
}

void CursorComponent::Reset()
{
    mAlive = true;
    mCurrentResponse.clear();
    mRealResponse.clear();
}

void CursorComponent::SetDrunk(bool val)
{
    mIsDrunk =  val;

    if(mIsDrunk)
    {
        //mAcceleration = 600;
    }
}

void CursorComponent::ToggleInput(bool takeInput)
{
    mTakeInput = takeInput;    
}

void CursorComponent::Update(float dTime) 
{
    mTimeSinceBirth += dTime;
    std::vector<std::shared_ptr<Opal::Entity> > ents = Opal::Scene::GetActiveScene()->GetAllEntities();


    mAttractors = mAttractable->GetFrameAttractors();

    UpdateVerts();

    bool takingInput = false;

    if(mTakeInput)
    {
        takingInput = false;

        float yMovement = 0;
        float yPosNorm = mTransform->Position.y / (float)Opal::Game::Instance->GetHeight();

        bool upPressed = false;
        bool downPressed = false;
        if(Opal::InputHandler::GetKey(mUpBinding) || Opal::InputHandler::GetKey(mUpBindingAlt))
        {
            upPressed = true;
            takingInput = !takingInput;
            yMovement = -1.0f;
        }
        else if(Opal::InputHandler::GetLeftJoystickY() > 0.3)
        {
            takingInput = true;
            yMovement = Opal::InputHandler::GetLeftJoystickY();
        }

        if(Opal::InputHandler::GetKey(mDownBinding) || Opal::InputHandler::GetKey(mDownBindingAlt))
        {
            downPressed = true;
            takingInput = !takingInput;
            yMovement = 1.0f;
        }
        else if(Opal::InputHandler::GetLeftJoystickY() < -0.3)
        {
            takingInput = true;
            yMovement = Opal::InputHandler::GetLeftJoystickY();
        }
        
        float prevSpeed = mCurrentSpeed;
        mCurrentSpeed += mAcceleration * dTime * yMovement;

        if ((prevSpeed >= 0 && mCurrentSpeed < 0 && !downPressed) || (prevSpeed <= 0 && mCurrentSpeed >0 && !upPressed))
        {
            int blow = (rand() % 5) + 1;
            std::string filename = "blow";
            filename.append(std::to_string(blow)).append(".wav");
            auto clip = AudioBank::Instance->GetClip(filename);
            Opal::Game::Instance->mAudioEngine.PlaySound(clip, 0.3f, 1.0f, 0.0f, false);
        }

        if(mCurrentSpeed > mMaxSpeed)
        {
            mCurrentSpeed = mMaxSpeed;
        }
        if(mCurrentSpeed < -mMaxSpeed)
        {
            mCurrentSpeed = -mMaxSpeed;
        }
    }

    if(mKilled)
    {
        takingInput = true;
        mCurrentSpeed += mAcceleration * 1.5f * dTime;
    }
    if(!takingInput)
        mCurrentSpeed = 0;

    auto vel = mParent->GetComponent<Opal::VelocityComponent>();
    vel->SetVelocity(vel->GetVelocity() + glm::vec3(0,mCurrentSpeed, 0));

    mLastSpeed = mCurrentSpeed;
}

void CursorComponent::Kill()
{
    mKilled = true;
}

bool CursorComponent::GetKill()
{
    return mKilled;
}

void CursorComponent::Render(std::shared_ptr<Opal::BatchRenderer2D>   ctx) 
{
    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplGlfw_NewFrame();

    // ImGui::NewFrame();

    // ImGui::Begin("Player Info");

    // ImGui::DragFloat("Max Radius", &mMaxRadius);

    // ImGui::End();
    // ImGui::Render();
    // ImGui::EndFrame();
}

bool CursorComponent::GetAlive()
{
    return mAlive;
}

std::vector<std::string> CursorComponent::GetResponse(bool onlyCore)
{
    return (onlyCore) ? mRealResponse : mCurrentResponse;
}

void CursorComponent::OnCollision(std::shared_ptr<Opal::Entity> other, glm::vec2 resolution, Opal::AABB otherAABB) 
{
    if(other->GetComponent<EndWallComponent>() != nullptr)
    {
        mAlive = false;
    }

    std::shared_ptr<SentenceFragmentComponent> otherComp = other->GetComponent<SentenceFragmentComponent>(); 
    if(otherComp != nullptr && otherComp->GetActive() && otherComp->GetSolid())
    {
        mCurrentResponse.push_back(otherComp->Text);
        
        if(!otherComp->IsIntrusive)
        {
            mRealResponse.push_back(otherComp->Text);
        }
        otherComp->Interact();

        mShouldPop = true;

        int snap = (rand() % 3) + 1;
        std::string filename = "snap";
        filename.append(std::to_string(snap)).append(".wav");
        auto clip = AudioBank::Instance->GetClip(filename);
        if (clip != nullptr)
            Opal::Game::Instance->mAudioEngine.PlaySound(clip, 0.5f, 1.0f, 0.0f, false);
        else
            std::cout << filename << std::endl;
    }
}

void CursorComponent::SetAlive(bool val)
{
    mAlive = val;
}

void CursorComponent::Serialize() 
{

}

void CursorComponent::Deserialize() 
{

}

std::shared_ptr<Opal::Component> CursorComponent::Clone() 
{
    return std::make_shared<CursorComponent>();
}

std::shared_ptr<Opal::Mesh2D> CursorComponent::GetMesh()
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
                attraction += dotprodRaised * mAttractionModifier * ((mAttractors[j].Strength)/mAttractionScale);
            }            
        }

        float newRadius = mRadius + noiseVal + attraction;

        float circleProgress = (float)i / (float)mNumTris; 
        if(circleProgress > 0.8)
        {
            newRadius = Opal::OpalMath::Approach(mRadius + noiseVal + attraction, firstRadius, (circleProgress - 0.8f) * 5.0f);
        }

        if(newRadius > mMaxRadius)
            newRadius = mMaxRadius;

        if(mLastRadii[i] > 0)
        {
            newRadius = Opal::OpalMath::Approach(mLastRadii[i], newRadius, mLerpFactor);
        }

        mLastRadii[i] = newRadius;

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
