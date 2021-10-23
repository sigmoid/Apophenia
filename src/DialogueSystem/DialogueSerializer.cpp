#include "DialogueSerializer.h"


#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

std::vector<std::string> DialogueSerializer::DeserializeStoryline(std::string filepath)
{
    std::vector<std::string> res;

    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath.c_str());

    std::string base = doc.RootElement()->Attribute("Filepath");

    //Todo: Not yet used for anything
    std::string storyName = doc.RootElement()->Attribute("Story");

    for(tinyxml2::XMLElement *convo = doc.RootElement()->FirstChildElement(); convo != nullptr; convo = convo->NextSiblingElement())
    {
        res.push_back(base + convo->GetText());
    }

    return res;
}

std::vector<Prompt> DialogueSerializer::DeserializeFile(std::string filepath)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath.c_str());

    doc.PrintError();
    
    std::vector<Prompt> res;
    for(tinyxml2::XMLElement *pmpt = doc.RootElement()->FirstChildElement(); pmpt != nullptr; pmpt = pmpt->NextSiblingElement())
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
    res.Sequence = root->IntAttribute("Sequence", -1);
    const char * tmp = root->Attribute("TransitionText");
    if(tmp != nullptr)
        res.TransitionText = tmp; 
    res.Text = root->FirstChildElement("Text")->GetText();
    std::replace(res.Text.begin(), res.Text.end(), '\t', ' ');
    res.Response = DeserializeResponse(root->FirstChildElement("Response"));
    tinyxml2::XMLElement *colorRoot = root->FirstChildElement("Color");
    res.Color.r = colorRoot->FirstChildElement("r")->FloatText();
    res.Color.g = colorRoot->FirstChildElement("g")->FloatText();
    res.Color.b = colorRoot->FirstChildElement("b")->FloatText();
    res.Color.a = colorRoot->FirstChildElement("a")->FloatText();

    for(tinyxml2::XMLElement *tgr = root->FirstChildElement("Triggers")->FirstChildElement(); tgr != nullptr; tgr = tgr->NextSiblingElement())
    {
        res.Triggers.push_back(tgr->GetText());
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

    res.ChannelSize = root->FloatAttribute("ChannelSize", 700.0f);

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

    res.Text = root->FirstChildElement("Text")->GetText();
    res.Attraction = root->FirstChildElement("Attraction")->FloatText();
    res.IsIntrusive = root->BoolAttribute("IsIntrusive", false);
    return res;
} 
