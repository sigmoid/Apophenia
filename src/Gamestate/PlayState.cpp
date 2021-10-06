#include "PlayState.h"
#include "SentenceFormingState.h"
#include "../../Opal/Game.h"
#include "../../Opal/Input/InputHandler.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/Prompt.h"
#include "../../Opal/OpalMath.h"
#include "../DialogueSystem/DialogueSerializer.h"

PlayState::PlayState()
{

}

void PlayState::Tick() 
{
    if(mTransitionTimer > 0)
    {
        mTransitionTimer -= mGame->GetDeltaTime();
        UpdateColor();
    }
    else
    {
        if(Opal::InputHandler::GetKey(GLFW_KEY_SPACE))
        {
            if(DialogueManager::Instance->GetCurrentPrompt().IsEnd)
            {
                IncrementConversation();
            }
            else
            {
                mGame->PushState<SentenceFormingState>();
            }
        }
    }
}

void PlayState::Render() 
{
    if(mTransitionTimer > 0)
    {
        mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().TransitionText, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    }
    else
    {
        mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    }

    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void PlayState::Begin() 
{
    mConversationSequence = DialogueSerializer::DeserializeStoryline("../Dialogue/DialogueProgression.xml");
    mTextPass = mGame->Renderer->CreateRenderPass(true);

    Opal::Font typeFace(mGame->Renderer,"../fonts/JosefinSans-Light.ttf", 120);

    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(mGame->GetWidth() - 300, mGame->GetHeight()), Opal::Camera::ActiveCamera);
    IncrementConversation();
    UpdateColor();
}

void PlayState::IncrementConversation()
{
    if(mCurrentConversation > 0)
        mPreviousColor = DialogueManager::Instance->GetCurrentPrompt().Color;
   
    if(mCurrentConversation >= mConversationSequence.size())
    {
        std::cout << "GAME OVER" << std::endl;
        //todo, figure out what to do here
        return;
    }

    DialogueManager::Instance->LoadConversation(mConversationSequence[mCurrentConversation++]);
    mTransitionTimer = mTransitionDuration;
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

    if(mTransitionTimer > 0)
    {
        color.r = Opal::OpalMath::Lerp(mPreviousColor.r, color.r, 1 - mTransitionTimer / mTransitionDuration);
        color.g = Opal::OpalMath::Lerp(mPreviousColor.g, color.g, 1 - mTransitionTimer / mTransitionDuration);
        color.b = Opal::OpalMath::Lerp(mPreviousColor.b, color.b, 1 - mTransitionTimer / mTransitionDuration);
    }

    mTextPass->SetClearColor(color.r, color.g, color.b, color.a);
}
