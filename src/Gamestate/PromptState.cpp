#include "PromptState.h"
#include "SentenceFormingState.h"
#include "../../Opal/Game.h"
#include "../../Opal/Input/InputHandler.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/Prompt.h"
#include "../../Opal/OpalMath.h"
#include "../../Opal/Logger.h"
#include "../../Opal/PlayerPrefs.h"
#include "../DialogueSystem/DialogueSerializer.h"
#include "EndState.h"
#include "PillState.h"
#include "StrikesState.h"
#include "DrawingState.h"

std::shared_ptr<Opal::FontRenderer> PromptState::mTextRenderer = nullptr;
std::shared_ptr<Opal::RenderPass> PromptState::mTextPass = nullptr;
std::shared_ptr<Opal::PostProcessRenderer> PromptState::mPostProcessor = nullptr;
std::shared_ptr<Opal::Texture> PromptState::mTextureOutput = nullptr;
std::shared_ptr<Opal::Font> PromptState::mFont = nullptr;
std::shared_ptr<Opal::SpriteRenderer> PromptState::mSpriteRenderer = nullptr;
std::shared_ptr<Opal::BatchRenderer2D> mBatchRenderer;

PromptState::PromptState()
{

}

PromptState::~PromptState()
{
    
}

void PromptState::Tick() 
{

    
    auto prompt = DialogueManager::Instance->GetCurrentPrompt();

    if(prompt.Text[0] == "BLANK")
    {
        mGame->PopState();
        return;
    }

    if(prompt.Sound != "" && !mHasSound && !mSoundThisPrompt)
    {
        mCurrentAudioClip = Opal::AudioEngine::LoadClip(prompt.Sound);
        mSoundTimer = prompt.SoundDelay;
        mHasSound = true;
        mSoundThisPrompt = true;
    }
    else if(prompt.Sound == "")
    {
        mSoundThisPrompt = false;
    }

    mNoiseOffset += mGame->GetDeltaTime()/8.0f;
    
    if(mColorWarpTimer > 0)
    {
        mColorWarpTimer -= mGame->GetDeltaTime();
        UpdateColor(1 - mColorWarpTimer / mColorWarpPeriod);

        if(mColorWarpTimer <= 0)
        {
            mColorWarpTimer = mColorWarpPeriod;
            mPreviousColor = mPreviousColor = DialogueManager::Instance->GetCurrentPrompt().Colors[mCurrentColorId];
            mCurrentColorId = rand() % DialogueManager::Instance->GetCurrentPrompt().Colors.size();
        }
    }

    if (mTutorialTimer > 0)
        mTutorialTimer -= mGame->GetDeltaTime();

    if((Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE) || Opal::InputHandler::GetTouch() || Opal::InputHandler::GetGamepadButton(0)) && !mLastAdvancePressed)
    {
        if (!mHasPlayedTutorial)
        {
            Opal::PlayerPrefs::SaveBool("HasPlayedDialogueTutorial", true);
            mTutorialTexture = nullptr;
        }

        if(mCurrentCardIdx < DialogueManager::Instance->GetCurrentPrompt().Text.size() - 1)
        {
            mCurrentCardIdx++;
        }
        else
        {
            if(DialogueManager::Instance->GetCurrentPrompt().IsEnd)
            {
                mGame->PopState();
                return;
            }
            else
            {
                mGame->PushState<SentenceFormingState>();
            }
        }
    }
    mLastAdvancePressed = Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE) || Opal::InputHandler::GetGamepadButton(0);

    if(mSoundTimer > 0)
    {
        mSoundTimer -= mGame->GetDeltaTime();
    }
    
    if(mSoundTimer <= 0 && mHasSound && mSoundThisPrompt)
    {
        mHasSound = false;
        mGame->mAudioEngine.PlaySound(mCurrentAudioClip, 0.8f, 1.0f, 0.0f, false, false);
    }
    
}

void PromptState::Render() 
{
    #ifdef __IPHONEOS__
    mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text[mCurrentCardIdx], Opal::Game::Instance->GetWidth() * 0.15, Opal::Game::Instance->GetHeight()/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
    #else
    mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text[mCurrentCardIdx], 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
    #endif
    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mSpriteRenderer->StartFrame();
    if (mTutorialTimer <= 0 && mTutorialTexture != nullptr)
    {
        Opal::Sprite tutorialSprite(mTutorialTexture);
        tutorialSprite.SetScale(0.25f, 0.25f);
        tutorialSprite.SetPosition(1920 - tutorialSprite.GetSize().x, 1080 - tutorialSprite.GetSize().y);
        mSpriteRenderer->Submit(tutorialSprite);
    }
    mSpriteRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);

    UpdateShaderData();

    mPostProcessor->ProcessAndSubmit();

    //     ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplGlfw_NewFrame();

    // ImGui::NewFrame();

    // ImGui::Begin("Noise settings");

    // ImGui::DragFloat("Frequency", &mNoiseFrequency, 0.05f);

    // ImGui::End();

    // ImGui::Render();
    // ImGui::EndFrame();
}

void PromptState::Begin() 
{
    Opal::Logger::LogString("GAMESTATE: Begin() PromptState");

    if(mTextRenderer == nullptr)
    {
        #ifdef __IPHONEOS__
        mTextureOutput = mGame->Renderer->CreateRenderTexture(Opal::Game::Instance->GetWidth(),Opal::Game::Instance->GetHeight(), 4);
        #else
        mTextureOutput = mGame->Renderer->CreateRenderTexture(1920,1080, 4);
        #endif
        mTextPass = mGame->Renderer->CreateRenderPass(mTextureOutput,true);

        mFont = std::make_shared<Opal::Font>(mGame->Renderer,Opal::GetBaseContentPath().append("fonts/JosefinSans-Light.ttf").c_str(), 80);

        mSpriteRenderer = mGame->Renderer->CreateSpriteRenderer(mTextPass);

        #ifdef __IPHONEOS__
        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mFont, glm::vec2(Opal::Game::Instance->GetWidth() * (0.8), Opal::Game::Instance->GetHeight()), Opal::Camera::ActiveCamera);
        #else
        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mFont, glm::vec2(1920 - 300, 1080), Opal::Camera::ActiveCamera);
        #endif
        mPostProcessor = mGame->Renderer->CreatePostProcessor(mTextPass, Opal::GetBaseContentPath().append("shaders/PlayStateVert"), Opal::GetBaseContentPath().append("shaders/PlayStateFrag"), true, sizeof(PromptStateShaderData), VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    mHasPlayedTutorial = Opal::PlayerPrefs::GetBool("HasPlayedDialogueTutorial");

    if (!mHasPlayedTutorial)
    {
        mTutorialTexture = mGame->Renderer->CreateTexture(Opal::GetBaseContentPath().append("textures/DialogueTutorial.png"));
    }

    mCurrentCardIdx = 0;
    mTutorialTimer = mTutorialWaitTime;
    UpdateShaderData();
    UpdateColor(1.0f);
}

void PromptState::End() 
{

}

void PromptState::Resume() 
{
    Opal::Logger::LogString("GAMESTATE: Resume() PromptState");
    
    mCurrentCardIdx = 0;

    

    UpdateColor(1.0f);
}

void PromptState::UpdateColor(float progress)
{
    glm::vec4 color;

    if(!IsPillSequence && !IsDrawingScene)
    {
        color = DialogueManager::Instance->GetCurrentPrompt().Colors[mCurrentColorId];
    }
    else
    {
        color = glm::vec4(0.2,0.2,0.2,1.0f);
    }

    if(progress > 0 )
    {
        color.r = Opal::OpalMath::Lerp(mPreviousColor.r, color.r, (progress > 1.0f) ? 1.0f : progress);
        color.g = Opal::OpalMath::Lerp(mPreviousColor.g, color.g, (progress > 1.0f) ? 1.0f : progress);
        color.b = Opal::OpalMath::Lerp(mPreviousColor.b, color.b, (progress > 1.0f) ? 1.0f : progress);
    }

    mTextPass->SetClearColor(color.r, color.g, color.b, color.a);
}

void PromptState::UpdateShaderData()
{
     
    mShaderData = new PromptStateShaderData();

    for(int i = 0; i < 8; i++)
    {
        int sz = DialogueManager::Instance->GetCurrentPrompt().ObscuredWords.size();
        if(i > sz - 1)
        {
            mShaderData->ObscuredRects[i] = glm::vec4(0,0,0,0);
        }
        else
        {
            if(DialogueManager::Instance != nullptr)
            {
                Opal::AABB box = mTextRenderer->GetTextRect(DialogueManager::Instance->GetCurrentPrompt().Text[mCurrentCardIdx],DialogueManager::Instance->GetCurrentPrompt().ObscuredWords[i] , 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
                    
                    
                // std::cout << box.min.x << " " << box.min.y << " " << box.max.x << " " << box.max.y << std::endl;
                mShaderData->ObscuredRects[i] = glm::vec4(box.min.x / 1920, box.min.y / 1080, box.max.x / 1920, box.max.y / 1080);
            }
        }
        
    }

    mPostProcessor->UpdateUserData((void *)mShaderData);
}
