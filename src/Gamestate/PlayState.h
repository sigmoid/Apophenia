#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"

#include <array>

class PlayState : public Opal::Gamestate
{

    public:
    PlayState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:

    void IncrementConversation();

    Opal::FontRenderer *mTextRenderer;
    Opal::RenderPass *mTextPass;

    std::vector<std::string> mConversationSequence;

    int mCurrentConversation = 0;

    float mTransitionTimer = 0, mTransitionDuration = 1;
    glm::vec4 mPreviousColor = glm::vec4(101.0f/255.0f, 130.0f/255.0f, 191.0f/255.0f, 1);

    void UpdateColor();
};