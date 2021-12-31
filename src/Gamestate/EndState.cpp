#include "EndState.h"
#include "../../Opal/Logger.h"
#include "../../Opal/Game.h"

std::shared_ptr<Opal::FontRenderer> EndState::mTextRenderer = nullptr;
std::shared_ptr<Opal::RenderPass> EndState::mTextPass = nullptr;

EndState::EndState()
{

}

EndState::~EndState()
{
    
}

void EndState::Tick()
{

}

void EndState::Render()
{
    mTextPass->Record();

    mTextRenderer->RenderString("The End of \"Tightrope\" for now...\n"
    "The final act hasn't been written yet, but will cover getting to stability.\n"
    "follow @absurd_walls on twitter to send me feedback, and find out when it's finished\n"
    "Thanks for playing."
    ,
     0,0,1,1,1,1,1,true);
    mTextRenderer->RecordCommands();

    mTextPass->EndRecord();
    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void EndState::Begin()
{
    Opal::Logger::LogString("GAMESTATE: Begin() EndState");

    if(mTextRenderer == nullptr)
    {
        mTextPass = mGame->Renderer->CreateRenderPass(true);
        mTextPass->SetClearColor(0.1,0.1,0.1,1);
        Opal::Font mFont(mGame->Renderer, "../fonts/JosefinSlab-SemiBold.ttf", 48);
        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, mFont, glm::vec2(1920, 1080), Opal::Camera::ActiveCamera);
    }
}

void EndState::End()
{
    Opal::Logger::LogString("GAMESTATE: End() EndState");
}

void EndState::Resume()
{

}
