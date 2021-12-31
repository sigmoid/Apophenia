#include "PromptState.h"
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

std::shared_ptr<Opal::FontRenderer> PromptState::mTextRenderer = nullptr;
std::shared_ptr<Opal::RenderPass> PromptState::mTextPass = nullptr;
std::shared_ptr<Opal::PostProcessRenderer> PromptState::mPostProcessor = nullptr;
std::shared_ptr<Opal::Texture> PromptState::mTextureOutput = nullptr;
std::shared_ptr<Opal::Font> PromptState::mFont = nullptr;
std::shared_ptr<Opal::SpriteRenderer> PromptState::mSpriteRenderer = nullptr;

PromptState::PromptState()
{

}

PromptState::~PromptState()
{
    
}

void PromptState::Tick() 
{

    
    auto prompt = DialogueManager::Instance->GetCurrentPrompt();

    if(prompt.Text == "BLANK")
    {
        mGame->PopState();
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

    if(Opal::InputHandler::GetKey(GLFW_KEY_SPACE) || Opal::InputHandler::GetGamepadButton(0))
    {
        if(DialogueManager::Instance->GetCurrentPrompt().IsEnd)
        {
            mGame->PopState();
        }
        else
        {
            mGame->PushState<SentenceFormingState>();
        }
    }

    if(mSoundTimer > 0)
    {
        mSoundTimer -= mGame->GetDeltaTime();
    }
    
    if(mSoundTimer <= 0 && mHasSound && mSoundThisPrompt)
    {
        mHasSound = false;
        mGame->mAudioEngine.PlaySound(mCurrentAudioClip, 0.2f, 1.0f, 0.0f, false, false);
    }
    
}

void PromptState::Render() 
{
    mTextRenderer->RenderString(DialogueManager::Instance->GetCurrentPrompt().Text, 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);

    mTextPass->Record();
    mTextRenderer->RecordCommands();
    mSpriteRenderer->StartFrame();
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
        mTextureOutput = mGame->Renderer->CreateRenderTexture(1920,1080, 4);
        mTextPass = mGame->Renderer->CreateRenderPass(mTextureOutput,true);

        mFont = std::make_shared<Opal::Font>(mGame->Renderer,"../fonts/JosefinSans-Light.ttf", 80);

        mSpriteRenderer = mGame->Renderer->CreateSpriteRenderer(mTextPass);

        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mFont, glm::vec2(1920 - 300, 1080), Opal::Camera::ActiveCamera);
        
        mPostProcessor = mGame->Renderer->CreatePostProcessor(mTextPass, "../shaders/PlayStateVert", "../shaders/PlayStateFrag", true, sizeof(PromptStateShaderData), VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    UpdateShaderData();
    UpdateColor(1.0f);
}

void PromptState::End() 
{

}

void PromptState::Resume() 
{
    Opal::Logger::LogString("GAMESTATE: Resume() PromptState");

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

    if(progress > 0)
    {
        color.r = Opal::OpalMath::Lerp(mPreviousColor.r, color.r, progress);
        color.g = Opal::OpalMath::Lerp(mPreviousColor.g, color.g, progress);
        color.b = Opal::OpalMath::Lerp(mPreviousColor.b, color.b, progress);
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
            Opal::AABB box = mTextRenderer->GetTextRect(DialogueManager::Instance->GetCurrentPrompt().Text,DialogueManager::Instance->GetCurrentPrompt().ObscuredWords[i] , 150, 1080/2- 60, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f, true);
            // std::cout << box.min.x << " " << box.min.y << " " << box.max.x << " " << box.max.y << std::endl;
            mShaderData->ObscuredRects[i] = glm::vec4(box.min.x / 1920, box.min.y / 1080, box.max.x / 1920, box.max.y / 1080);
            }
        }
        
    }

    mPostProcessor->UpdateUserData((void *)mShaderData);
}
