#pragma once

#include <vector>
#include <string>

#include "SentenceFragment.h"
#include "Response.h"
#include "Prompt.h"

class DialogueManager
{
    public:
    DialogueManager(std::string filepath);
    // returns 'true' if the response was valid
    bool ProcessResponse(std::string response);
    Prompt GetCurrentPrompt();
    Response GetCurrentResponse();

    static DialogueManager *Instance;

    private:
    int mCurrentPromptIdx;
    Response mCurrentResponse;

    std::vector<Prompt> mPrompts;

    void LoadGame(std::string filepath);
};