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

    mTextRenderer->RenderString("The End of \"Tightrope\" for now...\n One thing I want to note that bugs me. I exaggerated the hallucinations to make the story more understandable. \n All of the conversations with the hallucination guy were trains of thought that went on in my own head. I did hallucinate that there was a person trying to get me out, but that was a minor part of the experience. Thanks for playing. Let me know what you think. ",
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
        Opal::Font mFont(mGame->Renderer, "../fonts/JosefinSans.ttf", 48);
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
