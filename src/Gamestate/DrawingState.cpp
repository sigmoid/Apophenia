#include "DrawingState.h"
#include "../Opal/EntityComponent/EntityComponentSerialization.h"
#include "../Opal/EntityComponent/TransformComponent.h"
#include "../Opal/EntityComponent/BoxColliderComponent2D.h"

std::shared_ptr<Opal::LineRenderer> DrawingState::mLineRenderer = nullptr;
std::shared_ptr<Opal::RenderPass> DrawingState::mRenderPass = nullptr;
float DrawingState::Duration = 1;
std::string DrawingState::DrawingPath = "../Drawings/Test";

DrawingState::DrawingState()
{

}

DrawingState::~DrawingState()
{
    
}

void DrawingState::Tick() 
{
    mTimer -= mGame->GetDeltaTime();

    if(mTimer <= 0 || Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE))
    {
        mGame->PopState();
        return;
    }
}

void DrawingState::Render() 
{
    bool draw = false;
    std::shared_ptr<Opal::LinePointComponent> lastPoint;
    for(auto & entity : mScene->GetAllEntities())
    {
        auto line = entity->GetComponent<Opal::LinePointComponent>();

        if(line == nullptr)
            continue;

        if(draw)
        {
            mLineRenderer->DrawLine(lastPoint->Position, line->Position, mLineColor, mLineColor, mLineWidth);
        }

        lastPoint = line;
        draw = !draw;
    }

    mRenderPass->Record();
    mLineRenderer->Render();
    mRenderPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mRenderPass);
}

void DrawingState::Begin() 
{
    mTimer = Duration;

    if(mLineRenderer == nullptr)
    {
        mRenderPass = mGame->Renderer->CreateRenderPass(true);
        mRenderPass->SetClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        mLineRenderer = std::make_shared<Opal::LineRenderer>();
        mLineRenderer->Init(mGame->Renderer,mRenderPass,true);

        Opal::Serializer::ComponentRegistry["TransformComponent"] = std::make_shared<Opal::TransformComponent>();    
        Opal::Serializer::ComponentRegistry["BoxColliderComponent2D"] = std::make_shared<Opal::BoxColliderComponent2D>();    
        Opal::Serializer::ComponentRegistry["LinePointComponent"] = std::make_shared<Opal::LinePointComponent>();
    }

    mBatch = mGame->Renderer->CreateBatch(mRenderPass, 100, std::vector<std::shared_ptr<Opal::Texture> >(), true);
    mScene = std::make_shared<Opal::Scene>(mBatch);
    mScene->Load(DrawingPath);
    mScene->SortEntities(LinePointCompare());
}

void DrawingState::End() 
{

}

void DrawingState::Resume() 
{

}
