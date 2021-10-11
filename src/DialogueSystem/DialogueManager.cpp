#include "DialogueManager.h"
#include "DialogueSerializer.h"
#include "../../Opal/Logger.h"

DialogueManager *DialogueManager::Instance = nullptr;

DialogueManager::DialogueManager(std::string filepath)
{
    Instance = this;
}

bool DialogueManager::IsConversationOver()
{
    return mPrompts[mCurrentPromptIdx].IsEnd;
}

// returns 'true' if the response was valid
bool DialogueManager::ProcessResponse(std::string response)
{
    for(int i = 0; i < mPrompts.size(); i++)
    {
        if(std::find(mPrompts[i].Triggers.begin(), mPrompts[i].Triggers.end(), response) != mPrompts[i].Triggers.end())
        {
            Opal::Logger::LogString("DIALOGUE_MANAGER: response \"" + response + "\" accepted");
            mCurrentPromptIdx = i;
            mCurrentResponse = mPrompts[i].Response;
            return true;
        }
    }
    return false;
}

Prompt DialogueManager::GetCurrentPrompt()
{
    return mPrompts[mCurrentPromptIdx];
}

Response DialogueManager::GetCurrentResponse()
{
    return GetCurrentPrompt().Response;
}

void DialogueManager::LoadConversation(std::string filepath)
{
    Opal::Logger::LogString("DIALOGUE_MANAGER: Loading conversation " + filepath + "...");
    mPrompts.clear();
    mPrompts = DialogueSerializer::DeserializeFile(filepath);
    mCurrentPromptIdx = 0;
    return;
}