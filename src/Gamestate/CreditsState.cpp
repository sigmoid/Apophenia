#include "CreditsState.h"
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
#include "StrikesState.h"
#include "DrawingState.h"

std::shared_ptr<Opal::FontRenderer> CreditsState::mTextRenderer = nullptr;
std::shared_ptr<Opal::FontRenderer> CreditsState::mTitleTextRenderer = nullptr;
std::shared_ptr<Opal::RenderPass> CreditsState::mTextPass = nullptr;
std::shared_ptr<Opal::Texture> CreditsState::mTextureOutput = nullptr;
std::shared_ptr<Opal::Font> CreditsState::mFont = nullptr;
std::shared_ptr<Opal::Font> CreditsState::mTitleFont = nullptr;
std::shared_ptr<Opal::BatchRenderer2D> CreditsState::mBatch = nullptr;
std::shared_ptr<Opal::Texture> CreditsState::mTitleTexture = nullptr;
Opal::Sprite CreditsState::mTitleSprite;

CreditsState::CreditsState()
{

}

CreditsState::~CreditsState()
{
    
}

void CreditsState::Tick() 
{
    
}

void CreditsState::Render() 
{
    bool textOnScreen = mScrollProgress > -mGame->GetHeight()  - mSpaceBetweenCards * mCredits.size();
    for(int i = 0; i < mCredits.size(); i++)
    {
        mCredits[i].Position = mCredits[i].Position + glm::vec2(0, -1) * mScrollSpeed * mGame->GetDeltaTime();
        mTitleTextRenderer->RenderString(mCredits[i].Title, mCredits[i].Position.x, mCredits[i].Position.y, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, false);
        mTextRenderer->RenderString(mCredits[i].Name, mCredits[i].Position.x, mCredits[i].Position.y + 100, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, false);
    }

    if(!textOnScreen 
    || (Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE) && !mLastSpace) || (Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_A) && !mLastA))
    {   
        Opal::Logger::LogString ("Exit CreditsState");
        mGame->PopState();
        return;
    }
    mLastSpace = Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE);
    mLastA = Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_A);

    mTitleSprite.SetPosition(mTitleSprite.GetPosition().x, mTitleSprite.GetPosition().y - mScrollSpeed * mGame->GetDeltaTime());
    // mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text[mCurrentCardIdx], Opal::Game::Instance->GetWidth() * 0.15, Opal::Game::Instance->GetHeight()/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
    mScrollProgress -= mScrollSpeed * mGame->GetDeltaTime();

    mBatch->StartBatch();
    mBatch->BatchSprite(mTitleSprite);
    mBatch->RenderBatch();

    mTextPass->Record();
    mBatch->RecordCommands();
    mTextRenderer->RecordCommands();
    mTitleTextRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);
}

void CreditsState::Begin() 
{
    Opal::Logger::LogString("GAMESTATE: Begin() CreditsState");

    if(mTextRenderer == nullptr)
    {
        mTextPass = mGame->Renderer->CreateRenderPass(true);
        mTextPass->SetClearColor(0.2f,0.2f, 0.2f, 1.0f);

        mFont = std::make_shared<Opal::Font>(mGame->Renderer,Opal::GetBaseContentPath().append("fonts/JosefinSans-Light.ttf").c_str(), 80);
        mTitleFont = std::make_shared<Opal::Font>(mGame->Renderer,Opal::GetBaseContentPath().append("fonts/JosefinSlab-SemiBold.ttf").c_str(), 86);
        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mFont, glm::vec2(mGame->GetWidth() * (1 - 0.15625f), mGame->GetHeight()), Opal::Camera::ActiveCamera);    
        mTitleTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mTitleFont, glm::vec2(mGame->GetWidth() * (1 - 0.15625f), mGame->GetHeight()), Opal::Camera::ActiveCamera); 
        mTitleTexture = mGame->Renderer->CreateTexture(Opal::GetBaseContentPath().append("textures/Title.png"));
        mTitleSprite.SetTexture(mTitleTexture);
        float titleSpriteX = mGame->GetWidth() / 2.0f - mTitleTexture->GetWidth() /2.0f;
        float titleSpriteY = 50;
        mTitleSprite.SetPosition(titleSpriteX,titleSpriteY);

        std::vector<std::shared_ptr<Opal::Texture> > textures;
        textures.push_back(mTitleTexture);
        mBatch = mGame->Renderer->CreateBatch(mTextPass, 1000, textures, true);
    }  

    for(int i = 0; i < mCredits.size(); i++)
    {
        mCredits[i].Position = glm::vec2(200, 1080 + i * mSpaceBetweenCards);
    }
}

void CreditsState::End() 
{
    Opal::Logger::LogString("GAMESTATE: End() CreditsState");
}

void CreditsState::Resume() 
{
    Opal::Logger::LogString("GAMESTATE: Resume() CreditsState");
}
