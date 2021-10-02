#include "PlayState.h"
#include "../../Opal/Game.h"

PlayState::PlayState()
{

}

void PlayState::Tick() 
{

}

void PlayState::Render() 
{
    mTextRenderer->RenderString("Hello, world!", 1920/2 - 300, 1080/2, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);

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
