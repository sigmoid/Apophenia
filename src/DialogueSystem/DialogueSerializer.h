#pragma once

#include "Prompt.h"
#include "Response.h"
#include "SentenceFragment.h"

#include "../../Opal/vendor/tinyxml2/tinyxml2.h"

class DialogueSerializer
{
    public:
    static std::vector<Prompt> DeserializeFile(std::string filepath);
    static Prompt DeserializePrompt(tinyxml2::XMLElement *root);
    static Response DeserializeResponse(tinyxml2::XMLElement *root);
    static SentenceFragment DeserializeFragment(tinyxml2::XMLElement *root);
};