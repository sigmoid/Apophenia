#include "ManagerState.h"
#include "SentenceFormingState.h"
#include "PromptState.h"
#include "../../Opal/Game.h"
#include "../../Opal/Input/InputHandler.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/Prompt.h"
#include "../../Opal/OpalMath.h"
#include "../../Opal/Logger.h"
#include "../DialogueSystem/DialogueSerializer.h"
#include "EndState.h"
#include "PillState.h"
#include "StrikesState.h"
#include "DrawingState.h"
#include "CreditsState.h"
#include "MainMenuState.h"
#include "../Opal/Logger.h"

ManagerState::ManagerState()
{

}

ManagerState::~ManagerState()
{
    
}

void ManagerState::Tick() 
{
    auto prompt = DialogueManager::Instance->GetCurrentPrompt();
    if(prompt.Text.size() > 0 && prompt.Text[0] == "BLANK")
    {
        IncrementConversation();
    }
    

    if(mTransitionTimer > 0)
    {
        mTransitionTimer -= mGame->GetDeltaTime();
        auto currentColor = UpdateColor(1 - mTransitionTimer / mCurrentStateDuration);

        if(mTransitionTimer <= 0)
        {
            if(mCurrentState == GameStateType::TITLE_STATE)
            {
                mPreviousColor = currentColor;
            }
            else
            {
                mPreviousColor = DialogueManager::Instance->GetCurrentPrompt().Colors[0];
            }
        }
    }
    else
    {
        switch(mCurrentState)
        {

            case GameStateType::PILL_STATE:
                mGame->PushState<PillState>();
                break;      
            
            case GameStateType::DRAWING_STATE:
                mGame->PushState<DrawingState>();
                break;    
            
            case GameStateType::PROMPT_STATE:
                
                mColorWarpTimer = mColorWarpPeriod;
                mGame->PushState<PromptState>();
                break;

            case GameStateType::STRIKES_STATE:
                mGame->PushState<StrikesState>();
                break;

            case GameStateType::TITLE_STATE:
                IncrementConversation();
                break;

            case GameStateType::END_STATE:
                mGame->PushState<EndState>();
                break;
            case GameStateType::CREDITS_STATE:
                mGame->PushState<MainMenuState>();
                mGame->PushState<CreditsState>();
                break;

            default:
                Opal::Logger::LogString("Unrecognized transition");
        }
    }
}

void ManagerState::Render() 
{
#ifdef __IPHONEOS__
    int textXPos = Opal::Game::Instance->GetWidth() * 0.15;
    int textYPos = Opal::Game::Instance->GetHeight()/2 - Opal::Game::Instance->GetHeight() * 0.03125;
#else
    int textXPos = 150;
    int textYPos = 1080/2 - 60;
#endif
    if(mTitleText != "")
    {
        mTextRenderer->RenderString(mTitleText, textXPos, textYPos, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
    }
    else if(mCurrentState == GameStateType::PROMPT_STATE)
    {
        mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().TransitionText, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    }

    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mSpriteRenderer->StartFrame();
    mSpriteRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void ManagerState::Begin() 
{
    Opal::Logger::LogString("GAMESTATE: Begin() ManagerState");

    mPreviousColor = glm::vec4(0.1f,0.1f,0.1f, 1.0f);

    Opal::Logger::LogString("Loading Conversation...");

    mConversationSequence = DialogueSerializer::DeserializeStoryline(Opal::GetBaseContentPath().append("Dialogue/MainStory.xml"));

    Opal::Logger::LogString("Loading Progress...");
    LoadProgress();

    Opal::Logger::LogString("Creating Font...");
    mTextPass = mGame->Renderer->CreateRenderPass();
    Opal::Font typeFace(mGame->Renderer,Opal::GetBaseContentPath().append("fonts/JosefinSlab-SemiBold.ttf").c_str(), 100);

    Opal::Logger::LogString("Creating Sprite Renderer");
    mSpriteRenderer = mGame->Renderer->CreateSpriteRenderer(mTextPass);


    Opal::Logger::LogString("Creating Font Renderer...");
#ifdef __IPHONEOS__
    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(Opal::Game::Instance->GetWidth() *(.7), Opal::Game::Instance->GetHeight()), Opal::Camera::ActiveCamera);
#else
    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(1920 - 300, 1080), Opal::Camera::ActiveCamera);
#endif
    Opal::Logger::LogString("Incrementing Conversation...");
    IncrementConversation();

    if(mCurrentState != GameStateType::PROMPT_STATE)
    {
        int firstConvo = 0;
        while(true)
        {
            if(mConversationSequence[firstConvo].find("|") == std::string::npos )
            {
                break;
            }
            firstConvo++;
        }

        Opal::Logger::LogString("Loading Conversation...");
        DialogueManager::Instance->LoadConversation(mConversationSequence[firstConvo]);
    }

    Opal::Logger::LogString("Begin Complete.");
}

void ManagerState::IncrementConversation()
{
    mCurrentColorId = 0;

    // if(mCurrentConversation > 0)
    //     mPreviousColor = DialogueManager::Instance->GetCurrentPrompt().Colors[0];
   
    if(mCurrentConversation >= mConversationSequence.size())
    {
        mCurrentConversation = 1;
        SaveProgress();
        mPreviousState = mCurrentState;
        mCurrentState = GameStateType::CREDITS_STATE;
        return;
    }

    if(mConversationSequence[mCurrentConversation].find("|PillSequence") != std::string::npos)
    {
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
        
        mCurrentStateDuration = mTransitionDuration;
        mPreviousState = mCurrentState;
        mCurrentState = GameStateType::PILL_STATE;
        mCurrentConversation++;

        mTitleText = "";
    }
    else if(mConversationSequence[mCurrentConversation].find("|StrikesScreen") != std::string::npos)
    {
        mGame->PushState<StrikesState>();
        mPreviousState = mCurrentState;
        mCurrentStateDuration = mTransitionDuration;
        mCurrentState = GameStateType::STRIKES_STATE;
        mCurrentConversation++;
    }
    else if(mConversationSequence[mCurrentConversation].find("|DrawingState") != std::string::npos)
    {
        std::string data = mConversationSequence[mCurrentConversation];
        std::string firstPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + firstPart.length()+1);
        std::string secondPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + secondPart.length()+1);
        std::string thirdPart = data;

        float duration = std::stof(secondPart);
        std::string path = thirdPart;

        DrawingState::DrawingPath = path;
        DrawingState::Duration = duration;

        mCurrentStateDuration = mTransitionDuration;
        mPreviousState = mCurrentState;
        mCurrentState = GameStateType::DRAWING_STATE;
        mCurrentConversation++;

        mTitleText = "";
    }
    else if(mConversationSequence[mCurrentConversation].find("|TitleScreen") != std::string::npos)
    {
        std::string data = mConversationSequence[mCurrentConversation];
        std::string firstPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + firstPart.length()+1);
        std::string secondPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + secondPart.length()+1);
        std::string thirdPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + thirdPart.length()+1);
        std::string fourthPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + fourthPart.length()+1);
        std::string fifthPart = data.substr(0, data.find(" "));
        data.erase(data.begin(), data.begin() + fifthPart.length()+1);
        std::string sixthPart = data;

        float duration = std::stof(secondPart);
        mTitleText = sixthPart;
        mTitleText = ReplaceAll(mTitleText, "\\n", "\n");

        mCurrentStateDuration = duration;
        mTransitionTimer = duration;
        mPreviousState = mCurrentState;
        mCurrentState = GameStateType::TITLE_STATE;
        mCurrentConversation++;
    }
    else // Regular conversation
    {
        DialogueManager::Instance->LoadConversation(mConversationSequence[mCurrentConversation]);
        mTitleText = DialogueManager::Instance->GetCurrentPrompt().TransitionText;
        mCurrentStateDuration = mTransitionDuration;
        mPreviousState = mCurrentState;
        mCurrentState = GameStateType::PROMPT_STATE;

        if(mProgressLoaded)
            SaveProgress();
        else
            mProgressLoaded = true;
        
        mCurrentConversation++;
    }

    if(DialogueManager::Instance->EatTransition)
    {
        DialogueManager::Instance->EatTransition = false;
        UpdateColor(1.0f);
        mTransitionTimer = 0;
    }
    else if(mCurrentState == GameStateType::TITLE_STATE)
    {
        // transition timer already set
    }
    else
    {
        mTransitionTimer = mTransitionDuration;
    }
}

void ManagerState::End() 
{

}

void ManagerState::Resume() 
{
    Opal::Logger::LogString("GAMESTATE: Resume() ManagerState");

    if(mGameStarted && !mGamePaused && mCurrentState != GameStateType::TITLE_STATE)
        IncrementConversation();

    if (mGamePaused)
        mGamePaused = false;
}

void ManagerState::StartGame()
{
    mGameStarted = true;
}

void ManagerState::Pause()
{
    mGamePaused = true;
}

glm::vec4 ManagerState::UpdateColor(float progress)
{
    glm::vec4 color;

    if(mCurrentState != GameStateType::PILL_STATE && mCurrentState != GameStateType::DRAWING_STATE && mCurrentState != GameStateType::TITLE_STATE)
    {
        color = DialogueManager::Instance->GetCurrentPrompt().Colors[mCurrentColorId];
    }
    else if(mCurrentState == GameStateType::TITLE_STATE)
    {
        if(mConversationSequence[mCurrentConversation - 1].find("|TitleScreen") != std::string::npos)
        {
            std::string data = mConversationSequence[mCurrentConversation - 1];
            std::string firstPart = data.substr(0, data.find(" "));
            data.erase(data.begin(), data.begin() + firstPart.length()+1);
            std::string secondPart = data.substr(0, data.find(" "));
            data.erase(data.begin(), data.begin() + secondPart.length()+1);
            std::string thirdPart = data.substr(0, data.find(" "));
            data.erase(data.begin(), data.begin() + thirdPart.length()+1);
            std::string fourthPart = data.substr(0, data.find(" "));
            data.erase(data.begin(), data.begin() + fourthPart.length()+1);
            std::string fifthPart = data.substr(0, data.find(" "));
            data.erase(data.begin(), data.begin() + fifthPart.length()+1);
            std::string sixthPart = data;
            

            float r = std::stof(thirdPart);
            float g = std::stof(fourthPart);
            float b = std::stof(fifthPart);

            color = glm::vec4(r,g,b, 1.0f);
        }
        else
        {
            color = DialogueManager::Instance->GetCurrentPrompt().Colors[mCurrentColorId];
        }
    }
    else
    {
        color = glm::vec4(0.5,0.5,0.5,1.0f);
    }

    if(progress > 0)
    {
        color.r = Opal::OpalMath::Lerp(mPreviousColor.r, color.r, progress);
        color.g = Opal::OpalMath::Lerp(mPreviousColor.g, color.g, progress);
        color.b = Opal::OpalMath::Lerp(mPreviousColor.b, color.b, progress);
    }

    mTextPass->SetClearColor(color.r, color.g, color.b, color.a);

    return color;
}

std::string ManagerState::ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void ManagerState::LoadProgress()
{
    std::ifstream saveFile;
    saveFile.open(mSavePath, std::ifstream::in);
    if(!saveFile)
    {
        Opal::Logger::LogString("Failed to open save file!");
    }
    else
    {
        saveFile >> mCurrentConversation;
        saveFile.close();
    }
}

void ManagerState::SaveProgress()
{
    std::ofstream saveFile;
    saveFile.open(mSavePath, std::ofstream::out | std::ofstream::trunc);
    if(!saveFile)
    {
        Opal::Logger::LogString("Failed to open save file!");
    }
    else
    {
        saveFile << mCurrentConversation - 1;
        saveFile.close();
    }
}
