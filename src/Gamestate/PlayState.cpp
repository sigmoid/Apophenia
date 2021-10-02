#include "PlayState.h"
#include "SentenceFormingState.h"
#include "../../Opal/Game.h"
#include "../../Opal/Input/InputHandler.h"

PlayState::PlayState()
{

}

void PlayState::Tick() 
{
    if(Opal::InputHandler::GetKey(GLFW_KEY_SPACE))
    {
        mGame->PushState<SentenceFormingState>();
    }
}

void PlayState::Render() 
{
    mTextRenderer->RenderString("How are you feeling?", 1920/2 - 500, 1080/2, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);

    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void PlayState::Begin() 
{

    mTextPass = mGame->Renderer->CreateRenderPass(true);
    mTextPass->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Opal::Font typeFace(mGame->Renderer,"../fonts/JosefinSans.ttf", 120);

    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(mGame->GetWidth(), mGame->GetHeight()), Opal::Camera::ActiveCamera);
}

void PlayState::End() 
{

}

void PlayState::Resume() 
{
}
