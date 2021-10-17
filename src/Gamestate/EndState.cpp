#include "EndState.h"
#include "../../Opal/Logger.h"
#include "../../Opal/Game.h"

Opal::FontRenderer *EndState::mTextRenderer = nullptr;
Opal::RenderPass *EndState::mTextPass = nullptr;

EndState::EndState()
{

}

void EndState::Tick()
{

}

void EndState::Render()
{
    mTextPass->Record();

    mTextRenderer->RenderString("The End of \"Tightrope\" for now...\n Anxiety man will return in \n\"Anxiety Man 2: Bipolar Boogaloo\" \n (thanks for playing let me know what you think :) )", 0,0,1,1,1,1,1,true);
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
        Opal::Font mFont(mGame->Renderer, "../fonts/JosefinSans.ttf", 80);
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
