#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/Graphics/LineRenderer.h"
#include "../../Opal/Graphics/Texture.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "../../Opal/Graphics/MeshRenderer2D.h"
#include "../../Opal/Graphics/PostProcessRenderer.h"
#include "../Components/SparkComponent.h"
#include "../../Opal/Game.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../DialogueSystem/WordBank.h"

#include "../../Opal/EntityComponent/Scene.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <map>

struct UBO
{
    float warpFactor;
    float xPadding;
    float yPadding;
};

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
    static Opal::PostProcessRenderer *mPostProcess;
    static Opal::Texture *mRenderTexture;
    Opal::Scene *mScene = nullptr;
    Opal::Entity *mCursorEntity = nullptr;

    std::vector<glm::vec2> mLinePoints;
    glm::vec4 mLineColor = glm::vec4(0.9f, 0.9f, 0.9f, 0.8f);
    float mLineSpeed = 500;
    float mLineCutoff = -1;
    float mLineTimeStep = 0.05f, mLineTimer = 0;
    void UpdateCursorLine(float timeOverride = -1);
    void DrawCursorLine();

    float mKillEventTimer = 0;
    float mKillWaitTime = 3.4f;

    void CreatePlayingField();

    void PreBakeLines();

    void CreateSentenceFragment(glm::vec3 pos, std::string text, float attraction, float speed, bool intrusive, bool solid, bool core);
    void RenderSentenceFragments();
    glm::vec4 mFragmentColor = glm::vec4(0.9, 0.9, 0.9, 1.0f);
    std::vector<Opal::Entity *> mFragmentEnts;
    float mFragmentSize = 80;
    static Opal::Font *mFont;
    static Opal::Font *mResponseFont;

    float mNoiseFrequency = 12.5f / 2.0f;
    float mNoiseScaleJ = 5.0f;
    float mNoiseScale = 15.0f;

    float mSparkScale = 0.1f;
    float mSparkFreq = 0.95f;

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

    float mWarpFactor = 1.0f;
    float mBaseWarp = 1.0f, mMaxWarp = 1.45f;
    bool mIsWarped = false;

    float mZoomTimer = 0;
    float mZoomDuration = 10;
    float mCurrentZoom = 1;
    float mZoomTarget = 0.02f;

    float mZoom2Timer = 0;
    float mZoom2Duration = 6;
    float mZoom2Target = 0.01f;
    float mZoom2Pow = 0.5f;

    void RenderBlackHole();
    Opal::Mesh2D *mBHMesh;
    float mBHRadius = 370000;
    glm::vec2 mBHPos = glm::vec2(384000 + 80000, 1080/2);
    int mNumTris = 500;
    float mBHNoiseFreq = 0.002, mBHNoiseIntensity = 10000, mBHNoiseTurbulence = 100.0f;
    glm::vec4 mBHColor = glm::vec4(0,0,0,1);
    float mSparkSpeedUp = 1.0f;
    float mMaxSparkSpeedUp = 2.0f;

    float mTimeSinceBirth = 0;

    void StartScreenShake();
    float mScreenShakeTimer = 0;
    float mScreenShakeTime = 0.4f;
    int mScreenShakeIntensity = 30;

    std::vector<Word> mWordBank;
    std::vector<std::pair<Opal::Entity *, Opal::Entity *> > mWordConnections;
    float mSpaceBetweenFragments = 1920 * 0.7f;
    float mPadding = 50;
    FastNoiseLite mNoise;
    float mChannelHeight = 900;
    float mCurrentScroll = 0;
};