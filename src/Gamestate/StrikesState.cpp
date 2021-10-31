#include "StrikesState.h"
#include "../../Opal/Logger.h"
#include "../../Opal/Game.h"

Opal::FontRenderer *StrikesState::mTextRenderer = nullptr;
Opal::RenderPass *StrikesState::mTextPass = nullptr;

StrikesState::StrikesState()
{

}

void StrikesState::Tick()
{
    mTimer -= mGame->GetDeltaTime();

    mCurrentStrikes = 1 + (int)round(mMaxStrikes  * pow(1.0f - mTimer/mDuration, 4));
    mCurrentText = mDisplayText;
    for(int i = 0; i < mCurrentStrikes; i++)
    {
        mCurrentText += "I";
    }

    if(mTimer <= 0)
    {
        mGame->PopState();
        return;
    }
}

void StrikesState::Render()
{
    mTextPass->Record();

    mTextRenderer->RenderString(mCurrentText, 200,0,1,1,1,1,1,true);
    mTextRenderer->RecordCommands();

    mTextPass->EndRecord();
    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void StrikesState::Begin()
{
    Opal::Logger::LogString("GAMESTATE: Begin() StrikesState");

    if(mTextRenderer == nullptr)
    {
        mTextPass = mGame->Renderer->CreateRenderPass(true);
        mTextPass->SetClearColor(mBGColor.r, mBGColor.g, mBGColor.b, mBGColor.a);
        Opal::Font mFont(mGame->Renderer, "../fonts/JosefinSans.ttf", 180);
        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, mFont, glm::vec2(1920, 1080), Opal::Camera::ActiveCamera);
    }

    mCurrentText = mDisplayText;
    mTimer = mDuration;
}

void StrikesState::End()
{
    Opal::Logger::LogString("GAMESTATE: End() StrikesState");
}

void StrikesState::Resume()
{

}