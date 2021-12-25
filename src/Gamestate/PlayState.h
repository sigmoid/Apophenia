#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"
#include "../../Opal/Graphics/PostProcessRenderer.h"
#include "../../Opal/vendor/FastNoiseLite.h"

#include "../Opal/Audio/AudioClip.h"

#include <array>

struct PlayStateShaderData
{
    glm::vec4 ObscuredRects[8];
};

class PlayState : public Opal::Gamestate
{

    public:
    PlayState();
    ~PlayState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:

    void IncrementConversation();

    std::shared_ptr<Opal::FontRenderer> mTextRenderer;
    std::shared_ptr<Opal::RenderPass> mTextPass;
    std::shared_ptr<Opal::PostProcessRenderer> mPostProcessor;
    std::shared_ptr<Opal::Texture> mTextureOutput;

    std::vector<std::string> mConversationSequence;

    int mCurrentConversation = 0;

    float mTransitionTimer = 0, mTransitionDuration = 3.5f;
    float mColorWarpTimer = 0, mColorWarpPeriod = 1.0f;
    glm::vec4 mPreviousColor = glm::vec4(101.0f/255.0f, 130.0f/255.0f, 191.0f/255.0f, 1);
    std::shared_ptr<Opal::SpriteRenderer> mSpriteRenderer;
    bool IsPillSequence = false;
    int mCurrentColorId = 0;
    bool IsDrawingScene = false;

    float mSoundTimer = 0;

    PlayStateShaderData * mShaderData;

    void UpdateColor(float progress);

    std::shared_ptr<Opal::AudioClip> mCurrentAudioClip;
    bool mHasSound = false;
    bool mSoundThisPrompt = false;

    FastNoiseLite mNoise;
    float mNoiseFrequency = 1;
    float mNoiseOffset = 0;
    void UpdateShaderData();
};