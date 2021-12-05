#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"
#include <string>

class StrikesState : public Opal::Gamestate
{

    public:
    StrikesState();
    ~StrikesState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:

    static std::shared_ptr<Opal::FontRenderer> mTextRenderer;
    static std::shared_ptr<Opal::RenderPass> mTextPass;

    int mCurrentStrikes = 1;
    int mMaxStrikes = 6;

    std::string mDisplayText = "Strikes: ";
    std::string mCurrentText;
    glm::vec4 mBGColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    float mDuration = 5;
    float mTimer = 0;

};