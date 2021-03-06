#include "DialogueSerializer.h"


#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <Logger.cpp>

std::vector<std::string> DialogueSerializer::DeserializeStoryline(std::string filepath)
{
    std::vector<std::string> res;

    tinyxml2::XMLDocument doc;

    doc.LoadFile(filepath.c_str());

    tinyxml2::XMLElement *progressionRoot = doc.RootElement()->FirstChildElement("Progression");

    //std::string base = progressionRoot->Attribute("Filepath");

    //Todo: Not yet used for anything
    std::string storyName = progressionRoot->Attribute("Story");

    for(tinyxml2::XMLElement *convo = progressionRoot->FirstChildElement(); convo != nullptr; convo = convo->NextSiblingElement())
    {
        if(strcmp(convo->ToElement()->Value(), "Conversation") == 0)
        {
            res.push_back(convo->GetText());
        }
        else if(strcmp(convo->ToElement()->Value(), "PillSequence") == 0)
        {
            res.push_back("|PillSequence " + std::to_string(convo->FloatAttribute("NumPills", 9)) + " " + std::to_string(convo->FloatAttribute("Duration", 5)));
        }
        else if(strcmp(convo->ToElement()->Value(), "StrikesScreen") == 0)
        {
            res.push_back("|StrikesScreen");
        }
        else if(strcmp(convo->ToElement()->Value(), "DrawingState") == 0)
        {
            std::string duration = std::to_string(convo->FloatAttribute("Duration", 5));
            std::string path = convo->Attribute("FilePath");
            res.push_back("|DrawingState " + duration + " " + path);
        }
        else if(strcmp(convo->ToElement()->Value(), "TitleScreen") == 0)
        {
            std::string duration = std::to_string(convo->FloatAttribute("Duration", 3));
            std::string R = std::to_string(convo->FloatAttribute("R", 0.2f));
            std::string G = std::to_string(convo->FloatAttribute("G", 0.2f));
            std::string B = std::to_string(convo->FloatAttribute("B", 0.2f));
            res.push_back("|TitleScreen " + duration + " " + R + " " + G + " " + B + " "+ convo->GetText());
        }

    }

    return res;
}

std::vector<Prompt> DialogueSerializer::DeserializeFile(std::string filepath, std::string conversationName)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath.c_str());

    doc.PrintError();
    
    std::vector<Prompt> res;

    tinyxml2::XMLElement *conversationRoot = doc.RootElement()->FirstChildElement("Conversation");

    for(tinyxml2::XMLElement *pmpt = conversationRoot; pmpt != nullptr; pmpt = pmpt->NextSiblingElement())
    {
        std::string name = pmpt->Attribute("Name");
        if(name == conversationName)
        {
            conversationRoot = pmpt;
            break;
        }
    }

    for(tinyxml2::XMLElement *pmpt = conversationRoot->FirstChildElement(); pmpt != nullptr; pmpt = pmpt->NextSiblingElement())
    {
        res.push_back(DeserializePrompt(pmpt));
    }

    return res;
}

Prompt DialogueSerializer::DeserializePrompt(tinyxml2::XMLElement *root)
{
    Prompt res;
    res.IsEnd = root->BoolAttribute("IsEnd", false);
    res.IsKill = root->BoolAttribute("IsKill", false);
    res.IsZoomIn = root->BoolAttribute("IsZoomIn", false);
    res.Sequence = root->IntAttribute("Sequence", -1);
    res.InsideHead = root->BoolAttribute("InsideHead", false);
    auto sound = root->Attribute("Sound");
    if(sound != nullptr)
    {
        res.Sound = sound;
    }
    else
    {
        res.Sound = "";
    }
    res.SoundDelay = root->FloatAttribute("SoundDelay", 0);
    const char * tmp = root->Attribute("TransitionText");
    if(tmp != nullptr)
        res.TransitionText = tmp; 
    for(tinyxml2::XMLElement *txt = root->FirstChildElement("Text"); txt != nullptr; txt = txt->NextSiblingElement("Text"))
    {
        res.Text.push_back(txt->GetText());
        std::replace(res.Text[res.Text.size()-1].begin(), res.Text[res.Text.size()-1].end(), '\t', ' ');
    }
    res.Response = DeserializeResponse(root->FirstChildElement("Response"));

    for(tinyxml2::XMLElement *colorRoot = root->FirstChildElement("Color"); colorRoot != nullptr; colorRoot = colorRoot->NextSiblingElement("Color"))
    {
        glm::vec4 newColor;
        newColor.r = colorRoot->FirstChildElement("r")->FloatText();
        newColor.g = colorRoot->FirstChildElement("g")->FloatText();
        newColor.b = colorRoot->FirstChildElement("b")->FloatText();
        newColor.a = colorRoot->FirstChildElement("a")->FloatText();
        res.Colors.push_back(newColor);
    }

    for(tinyxml2::XMLElement *tgr = root->FirstChildElement("Triggers")->FirstChildElement(); tgr != nullptr; tgr = tgr->NextSiblingElement())
    {
        res.Triggers.push_back(tgr->GetText());
    }

    if(root->FirstChildElement("ObscuredWords") != nullptr)
    {
        for(tinyxml2::XMLElement *obsc = root->FirstChildElement("ObscuredWords")->FirstChildElement(); obsc != nullptr; obsc = obsc->NextSiblingElement())
        {
            if(obsc == nullptr)
            {
                break;
            }

            res.ObscuredWords.push_back(obsc->GetText());
        }
    }
    return res;
}
Response DialogueSerializer::DeserializeResponse(tinyxml2::XMLElement *root)
{
    Response res;
    tinyxml2::XMLElement *colorRoot = root->FirstChildElement("Color");
    res.Color.r = colorRoot->FirstChildElement("r")->FloatText();
    res.Color.g = colorRoot->FirstChildElement("g")->FloatText();
    res.Color.b = colorRoot->FirstChildElement("b")->FloatText();
    res.Color.a = colorRoot->FirstChildElement("a")->FloatText();
    res.Speed = root->FirstChildElement("Speed")->FloatText();

    res.SolidWords = root->BoolAttribute("SolidWords", false);
    res.AllowNonsense = root->BoolAttribute("AllowNonsense", false);
    res.RequireCoreOnly = root->BoolAttribute("RequireCoreOnly", false);
    res.ShowBlur = root->BoolAttribute("ShowBlur", false);
    res.Drunk = root->BoolAttribute("Drunk", false);
    res.Tremor = root->BoolAttribute("Tremor", false);

    if(root->Attribute("WordBank") != nullptr && root->Attribute("WordFrequency") != nullptr)
    {
        res.WordBank = root->Attribute("WordBank");
        res.WordFrequency = root->FloatAttribute("WordFrequency",0);
    }
    else
    {
        res.WordBank = "";
        res.WordFrequency = 0;
    }

    res.ChannelSize = root->FloatAttribute("ChannelSize", 500.0f);

    std::vector<std::vector<SentenceFragment> > wholeFragment;
    tinyxml2::XMLElement *fragRoot = root->FirstChildElement("Fragments");
    {
        for(tinyxml2::XMLElement * fgmt = fragRoot->FirstChildElement(); fgmt != nullptr; fgmt = fgmt->NextSiblingElement())
        {
            std::vector<SentenceFragment> currentFragment;
            for(tinyxml2::XMLElement * fgmt2 = fgmt->FirstChildElement(); fgmt2 != nullptr; fgmt2 = fgmt2->NextSiblingElement())
            {
                currentFragment.push_back(DeserializeFragment(fgmt2));
            }

            wholeFragment.push_back(currentFragment);
        }
    }

    res.Fragments = wholeFragment;

    return res;
}

std::vector<Word> DialogueSerializer::GetWordBank(std::string Filename)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(Filename.c_str());

    std::vector<Word> res;

    tinyxml2::XMLElement *docRoot = doc.RootElement();
    
    for(tinyxml2::XMLElement * wrd = docRoot->FirstChildElement("Word"); wrd != nullptr; wrd = wrd->NextSiblingElement("Word"))
    {
        Word curWord;

        tinyxml2::XMLElement *colorRoot = wrd->FirstChildElement("Color");
        if(colorRoot != nullptr)
        {
            curWord.Color.r = colorRoot->FirstChildElement("r")->FloatText();
            curWord.Color.g = colorRoot->FirstChildElement("g")->FloatText();
            curWord.Color.b = colorRoot->FirstChildElement("b")->FloatText();
            curWord.Color.a = colorRoot->FirstChildElement("a")->FloatText();
        }
        curWord.Speed = wrd->FloatAttribute("Speed", 1);
        curWord.Probability = wrd->FloatAttribute("Probability", 1);
        curWord.Text = wrd->GetText();            

        res.push_back(curWord);
    }
    

    return res;
}

SentenceFragment DialogueSerializer::DeserializeFragment(tinyxml2::XMLElement *root)
{
    SentenceFragment res;

    const char *text = root->FirstChildElement("Text")->GetText();
    if (text == nullptr)
        res.Text = "";
    else
        res.Text = text;
    
    res.Attraction = root->FirstChildElement("Attraction")->FloatText();
    res.IsIntrusive = root->BoolAttribute("IsIntrusive", false);
    return res;
} 
