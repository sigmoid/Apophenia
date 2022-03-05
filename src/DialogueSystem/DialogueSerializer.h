#pragma once

#include "Prompt.h"
#include "Response.h"
#include "WordBank.h"
#include "SentenceFragment.h"

#include "../../Opal/vendor/tinyxml2/tinyxml2.h"
#include <vector>
#include <string>

class DialogueSerializer
{
    public:
    static std::vector<std::string> DeserializeStoryline(std::string filepath);
    static std::vector<Prompt> DeserializeFile(std::string filepath, std::string conversationName);
    static Prompt DeserializePrompt(tinyxml2::XMLElement *root);
    static Response DeserializeResponse(tinyxml2::XMLElement *root);
    static SentenceFragment DeserializeFragment(tinyxml2::XMLElement *root);
    static std::vector<Word> GetWordBank(std::string Filename);
};