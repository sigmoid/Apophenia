#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"
#include "../../Opal/Graphics/PostProcessRenderer.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../../util.h"

#include "../Opal/Audio/AudioClip.h"

#include <array>

struct CreditsItem
{
    std::string Title;
    std::string Name;
    glm::vec2 Position;
};
class CreditsState : public Opal::Gamestate
{

    public:
    CreditsState();
    ~CreditsState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:
    std::vector<CreditsItem> mCredits {
        {"Design/Code/Writing", "Adam Waggoner"},
        {"Music", "Logan Richardson"},
        {"Libraries Used", "tinyxml2, SDL2, MoltenVK, Vulkan, glm, imgui, soloud, FastNoiseLite, stb_image"},
        {"Special Thanks", "Mom, Dad, Michael, Emily, Trevor, Logan, Light, Patrick, Skyler, Jaden, Sascha Willems"}
    };

    static std::shared_ptr<Opal::FontRenderer> mTextRenderer;
    static std::shared_ptr<Opal::FontRenderer> mTitleTextRenderer;
    static std::shared_ptr<Opal::RenderPass> mTextPass;
    static std::shared_ptr<Opal::Texture> mTextureOutput;
    static std::shared_ptr<Opal::Font> mFont;
    static std::shared_ptr<Opal::Font> mTitleFont;
    static std::shared_ptr<Opal::BatchRenderer2D> mBatch;
    static std::shared_ptr<Opal::Texture> mTitleTexture;

    static Opal::Sprite mTitleSprite;

    float mSpaceBetweenCards = 400;
    float mScrollSpeed = 75;

    float mScrollProgress = 0;

    bool mLastSpace = true;
};