#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/Graphics/LineRenderer.h"
#include "../../Opal/Graphics/Texture.h"
#include "../../Opal/Game.h"

#include "../../Opal/EntityComponent/Scene.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>

class SentenceFormingState : public Opal::Gamestate
{
    public:

    SentenceFormingState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:
    Opal::FontRenderer *mTextRenderer;
    Opal::RenderPass *mTextPass;
    Opal::Scene *mScene;
    Opal::BatchRenderer2D *mBatch;
    Opal::Texture *mCursorTexture;
    Opal::Entity *mCursorEntity;
    Opal::LineRenderer *mLineRenderer;

    std::vector<glm::vec2> mLinePoints;
    glm::vec4 mLineColor = glm::vec4(0.9f, 0.9f, 0.9f, 0.8f);
    float mLineSpeed = 500;
    float mLineCutoff = -1;
    float mLineTimeStep = 0.05f, mLineTimer;
    void UpdateCursorLine();
    void DrawCursorLine();

    void CreateSentenceFragment(glm::vec3 pos, std::string text);
    void RenderSentenceFragments();
    glm::vec4 mFragmentColor = glm::vec4(0.9, 0.9, 0.9, 1.0f);

    void CreatePlayer();

    void CreateEndWall(float x);
};