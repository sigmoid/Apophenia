#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"
#include "../../Opal/Graphics/PostProcessRenderer.h"
#include "../../Opal/Game.h"
#include "../../Opal/Graphics/Texture.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../../Opal/util.h"

#include <array>
struct NoiseData
{
    float noise[1920 * 1080];
};

class NarrativeState : public Opal::Gamestate
{

    public:
    NarrativeState();
    ~NarrativeState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:
    std::shared_ptr<Opal::Font> mFont;
    std::shared_ptr<Opal::FontRenderer> mFontRenderer;
    std::shared_ptr<Opal::RenderPass> mPass;
    std::shared_ptr<Opal::RenderPass> mBGPass;
    std::shared_ptr<Opal::Texture> mOutputTex;

    void GenerateNoise();
    float mFrequency = 1;
    float mTimeSinceBegin;
    float mNoiseScale = 1;
    NoiseData mNoiseValues;
    int mNoiseWidth = 1920, mNoiseHeight = 1080; 
    std::shared_ptr<Opal::PostProcessRenderer> mPostFX;
};
