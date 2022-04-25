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
#include "../../Opal/Audio/AudioClip.h"
#include "../../Opal/Audio/AudioClipInstance.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../DialogueSystem/WordBank.h"
#include "../Components/NoiseMoveComponent.h"

#include "../../Opal/EntityComponent/Scene.h"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <string>
#include <map>

enum MenuState {Default, Options};

class MainMenuState : public Opal::Gamestate
{

    public:
    MainMenuState();
    ~MainMenuState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:
    MenuState mCurrentState = MenuState::Default;
    std::shared_ptr<Opal::Scene> mScene = nullptr;
    std::shared_ptr<Opal::BatchRenderer2D> mBatch = nullptr;
    std::shared_ptr<Opal::LineRenderer> mLineRenderer = nullptr;

    int mNumSparks = 100;
    void CreateSparks();
    void CreateRandomSpark();
    void RenderSparks();
    std::vector<std::shared_ptr<Opal::Entity> > mSparkEntities;
    float mLineSpeed = 500;
    float mNoiseFrequency = 0.004f;
    float mNoiseScaleJ = 5.0f;
    float mNoiseScale = 15.0f;

    float mSparkScale = 0.01f;
    float mSparkFreq = 0.95f;
    
    std::shared_ptr<Opal::RenderPass> mRenderPass;

    bool firstUI = false;
};