#include "PillState.h"

Opal::MeshRenderer2D *PillState::mMeshRenderer = nullptr;
Opal::RenderPass *PillState::mRenderPass = nullptr;
int PillState::NumPills = 1;
float PillState::Duration = 1;

PillState::PillState()
{

}
void PillState::Tick() 
{
    mTimer -= mGame->GetDeltaTime();

    if(mTimer <= 0)
    {
        mGame->PopState();
        return;
    }

    for(auto & pill : mPills)
    {
        pill.Update(mGame->GetDeltaTime());
    }
}

void PillState::Render() 
{
    mMeshRenderer->StartFrame();
    for(auto & pill : mPills)
    {
        mMeshRenderer->Submit(pill.GetMesh());
    }

    mRenderPass->Record();
    mMeshRenderer->RecordCommands();
    mRenderPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mRenderPass);
}

void PillState::Begin() 
{
    mTimer = Duration;

    if(mMeshRenderer == nullptr)
    {
        mRenderPass = mGame->Renderer->CreateRenderPass(true);
        mRenderPass->SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        mMeshRenderer = mGame->Renderer->CreateMeshRenderer(mRenderPass);
    }

    for(int i = 0; i < NumPills; i++)
    {
        mPills.push_back(Pill(glm::vec2(rand() %1920, -(rand() % 100)), rand()));
    }
}

void PillState::End() 
{

}

void PillState::Resume() 
{

}
