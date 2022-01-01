#pragma once

#include <vector>
#include <string>

#include "SentenceFragment.h"
#include "Response.h"
#include "Prompt.h"
#include <iostream>
#include <fstream>

class DialogueManager
{
    public:
    DialogueManager(std::string filepath);
    // returns 'true' if the response was valid
    bool ProcessResponse(std::string response);
    void IncrementResponse();
    Prompt GetCurrentPrompt();
    Response GetCurrentResponse();
    bool IsConversationOver();
    void LoadConversation(std::string filepath);
    bool EatTransition = false;

    static DialogueManager *Instance;

    private:
    int mCurrentPromptIdx;
    int mCurrentSequenceNum = -1;

    std::vector<Prompt> mPrompts;

};