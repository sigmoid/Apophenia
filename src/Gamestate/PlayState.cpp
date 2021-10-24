#include "PlayState.h"
#include "SentenceFormingState.h"
#include "../../Opal/Game.h"
#include "../../Opal/Input/InputHandler.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/Prompt.h"
#include "../../Opal/OpalMath.h"
#include "../../Opal/Logger.h"
#include "../DialogueSystem/DialogueSerializer.h"
#include "EndState.h"
#include "PillState.h"

PlayState::PlayState()
{

}

void PlayState::Tick() 
{
    if(DialogueManager::Instance->GetCurrentPrompt().Text == "BLANK")
    {
        IncrementConversation();
    }

    if(mTransitionTimer > 0)
    {
        mTransitionTimer -= mGame->GetDeltaTime();
        UpdateColor();
    }
    else if (!IsPillSequence)
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
    else if (IsPillSequence)
    {
        mGame->PushState<PillState>();
    }
}

void PlayState::Render() 
{
    if(!IsPillSequence)
    {
        if(mTransitionTimer > 0)
        {
            mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().TransitionText, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
        }
        else
        {
            mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
        }
    }

    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mSpriteRenderer->StartFrame();
    mSpriteRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void PlayState::Begin() 
{
    Opal::Logger::LogString("GAMESTATE: Begin() PlayState");
    mConversationSequence = DialogueSerializer::DeserializeStoryline("../Dialogue/DialogueProgression.xml");
    mTextPass = mGame->Renderer->CreateRenderPass(true);

    Opal::Font typeFace(mGame->Renderer,"../fonts/JosefinSans-Light.ttf", 120);

    mSpriteRenderer = mGame->Renderer->CreateSpriteRenderer(mTextPass);

    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(1920 - 300, 1080), Opal::Camera::ActiveCamera);
    IncrementConversation();
    UpdateColor();
}

void PlayState::IncrementConversation()
{
    if(mCurrentConversation > 0)
        mPreviousColor = DialogueManager::Instance->GetCurrentPrompt().Color;
   
    if(mCurrentConversation >= mConversationSequence.size())
    {
        mGame->PushState<EndState>();
        return;
    }

    if(mConversationSequence[mCurrentConversation].find("|PillSequence") != std::string::npos)
    {
        IsPillSequence = true;

        std::string data = mConversationSequence[mCurrentConversation];
        std::string firstPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + firstPart.length()+1);
        std::string secondPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + secondPart.length()+1);
        std::string thirdPart = data;

        int numPills = std::stoi(secondPart);
        float duration = std::stof(thirdPart);

        PillState::NumPills = numPills;
        PillState::Duration = duration;
        mCurrentConversation++;
    }
    else
    {
        DialogueManager::Instance->LoadConversation(mConversationSequence[mCurrentConversation++]);
    }

    if(DialogueManager::Instance->EatTransition)
    {
        DialogueManager::Instance->EatTransition = false;
        UpdateColor();
    }
    else
    {
        mTransitionTimer = mTransitionDuration;
    }
}

void PlayState::End() 
{

}

void PlayState::Resume() 
{
    Opal::Logger::LogString("GAMESTATE: Resume() Playstate");

    if(IsPillSequence)
    {
        IsPillSequence = false;
        mTransitionTimer = mTransitionDuration;
        DialogueManager::Instance->LoadConversation(mConversationSequence[mCurrentConversation++]);
        mPreviousColor = glm::vec4(0.2,0.2, 0.2, 1.0f);
    }

    UpdateColor();
}

void PlayState::UpdateColor()
{
    glm::vec4 color;

    if(!IsPillSequence)
    {
        color = DialogueManager::Instance->GetCurrentPrompt().Color;
    }
    else
    {
        color = glm::vec4(0.2,0.2,0.2,1.0f);
    }

    if(mTransitionTimer > 0)
    {
        color.r = Opal::OpalMath::Lerp(mPreviousColor.r, color.r, 1 - mTransitionTimer / mTransitionDuration);
        color.g = Opal::OpalMath::Lerp(mPreviousColor.g, color.g, 1 - mTransitionTimer / mTransitionDuration);
        color.b = Opal::OpalMath::Lerp(mPreviousColor.b, color.b, 1 - mTransitionTimer / mTransitionDuration);
    }

    mTextPass->SetClearColor(color.r, color.g, color.b, color.a);
}
