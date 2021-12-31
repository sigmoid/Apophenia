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

enum class GameStateType
{
    PROMPT_STATE,
    SENTENCE_FORMING_STATE,
    PILL_STATE,
    END_STATE,
    STRIKES_STATE, 
    DRAWING_STATE,
    TITLE_STATE
};

class ManagerState : public Opal::Gamestate
{

    public:
    ManagerState();
    ~ManagerState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    void StartGame();

    private:

    void IncrementConversation();

    std::vector<std::string> mConversationSequence;

    int mCurrentConversation = 0;

    float mTransitionTimer = 0, mTransitionDuration = 3.5f;
    float mColorWarpTimer = 0, mColorWarpPeriod = 1.0f;
    glm::vec4 mPreviousColor = glm::vec4(101.0f/255.0f, 130.0f/255.0f, 191.0f/255.0f, 1);
    std::shared_ptr<Opal::FontRenderer> mTextRenderer;
    std::shared_ptr<Opal::RenderPass> mTextPass;
    std::shared_ptr<Opal::SpriteRenderer> mSpriteRenderer;
    int mCurrentColorId = 0;

    float mCurrentStateDuration = -1;
    bool mGameStarted = false;

    GameStateType mPreviousState, mCurrentState;

    std::string mTitleText;

    glm::vec4 UpdateColor(float progress);

    std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
};