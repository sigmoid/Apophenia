#include "PlayState.h"
#include "SentenceFormingState.h"
#include "../../Opal/Game.h"
#include "../../Opal/Input/InputHandler.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/Prompt.h"

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
    mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);

    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void PlayState::Begin() 
{

    mTextPass = mGame->Renderer->CreateRenderPass(true);
    UpdateColor();

    Opal::Font typeFace(mGame->Renderer,"../fonts/JosefinSans-Light.ttf", 120);

    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(mGame->GetWidth() - 300, mGame->GetHeight()), Opal::Camera::ActiveCamera);
}

void PlayState::End() 
{

}

void PlayState::Resume() 
{
    UpdateColor();
}

void PlayState::UpdateColor()
{
    glm::vec4 color = DialogueManager::Instance->GetCurrentPrompt().Color;
    mTextPass->SetClearColor(color.r, color.g, color.b, color.a);
}
