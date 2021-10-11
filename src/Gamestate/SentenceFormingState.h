#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/Graphics/LineRenderer.h"
#include "../../Opal/Graphics/Texture.h"
#include "../../Opal/Graphics/MeshRenderer2D.h"
#include "../Components/SparkComponent.h"
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
    static Opal::FontRenderer *mResponseRenderer;
    static Opal::RenderPass *mTextPass;
    static Opal::RenderPass *mBGPass;
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
    void UpdateCursorLine(float timeOverride = -1);
    void DrawCursorLine();

    void CreatePlayingField();

    void PreBakeLines();

    void CreateSentenceFragment(glm::vec3 pos, std::string text, float attraction, float speed, bool intrusive);
    void RenderSentenceFragments();
    glm::vec4 mFragmentColor = glm::vec4(0.9, 0.9, 0.9, 1.0f);
    std::vector<Opal::Entity *> mFragmentEnts;
    float mFragmentSize = 80;
    static Opal::Font *mFont;
    static Opal::Font *mResponseFont;

    float mNoiseFrequency = 12.5f;
    float mNoiseScaleJ = 5.0f;
    float mNoiseScale = 15.0f;

    void CreatePlayer();

    int mCOUNTER = 0;

    void CreateBounds();

    void CreateEndWall(float x);
    Opal::Entity *mEndWall;

    void RenderCurrentSelection();
    std::string ConcatSelection(std::vector<std::string> selection);

    void CreateSparks();
    int mNumSparks = 100;
    std::vector<Opal::Entity*> mSparkEntities;
    void RenderSparks();
    void CreateRandomSpark();

    void StartScreenShake();
    float mScreenShakeTimer = 0;
    float mScreenShakeTime = 0.4f;
    int mScreenShakeIntensity = 30;
};