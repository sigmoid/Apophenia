#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/Graphics/LineRenderer.h"
#include "../../Opal/Graphics/Texture.h"
#include "../../Opal/Graphics/MeshRenderer2D.h"
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
    static Opal::FontRenderer *mTextRenderer;
    static Opal::RenderPass *mTextPass;
    static Opal::BatchRenderer2D *mBatch;
    static Opal::MeshRenderer2D *mMeshRenderer;
    static Opal::Texture *mCursorTexture;
    static Opal::LineRenderer *mLineRenderer;
    Opal::Scene *mScene = nullptr;
    Opal::Entity *mCursorEntity = nullptr;

    std::vector<glm::vec2> mLinePoints;
    glm::vec4 mLineColor = glm::vec4(0.9f, 0.9f, 0.9f, 0.8f);
    float mLineSpeed = 500;
    float mLineCutoff = -1;
    float mLineTimeStep = 0.05f, mLineTimer = 0;
    void UpdateCursorLine();
    void DrawCursorLine();

    void CreatePlayingField();

    void CreateSentenceFragment(glm::vec3 pos, std::string text, float attraction, float speed);
    void RenderSentenceFragments();
    glm::vec4 mFragmentColor = glm::vec4(0.9, 0.9, 0.9, 1.0f);
    std::vector<Opal::Entity *> mFragmentEnts;
    float mFragmentSize = 80;
    static Opal::Font *mFont;

    void CreatePlayer();

    int mCOUNTER = 0;

    void CreateEndWall(float x);
    Opal::Entity *mEndWall;

    void RenderCurrentSelection();
    std::string ConcatSelection(std::vector<std::string> selection);

    void StartScreenShake();
    float mScreenShakeTimer = 0;
    float mScreenShakeTime = 0.4f;
    int mScreenShakeIntensity = 30;
};