#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"
#include "../../Opal/Graphics/BatchRenderer2D.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/LineRenderer.h"
#include "../../Opal/EntityComponent/Scene.h"
#include "../../Opal/EntityComponent/LinePointComponent.h"

struct LinePointCompare
{
    inline bool operator () (const std::shared_ptr<Opal::Entity>& a, const std::shared_ptr<Opal::Entity>& b)
    {
        std::shared_ptr<Opal::LinePointComponent> lpa = a->GetComponent<Opal::LinePointComponent>();
        std::shared_ptr<Opal::LinePointComponent> lpb = b->GetComponent<Opal::LinePointComponent>();

        return lpa->Id < lpb->Id;
    }
};

struct FXAAUBO
{
    float screenWidth;
    float screenHeight;
};


class DrawingState : public Opal::Gamestate
{

    public:
    DrawingState();
    ~DrawingState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    static float Duration;
    static std::string DrawingPath;

    private:

    float mTimer;

    glm::vec4 mLineColor = glm::vec4(1,1,1,1);
    float mLineWidth = 3;

    std::shared_ptr<Opal::Scene> mScene;
    std::shared_ptr<Opal::BatchRenderer2D> mBatch;

    static std::shared_ptr<Opal::LineRenderer> mLineRenderer;
    static std::shared_ptr<Opal::RenderPass> mRenderPass, mRenderPassToTexture;
    static std::shared_ptr<Opal::Texture> mRenderTexture;
    static std::shared_ptr<Opal::PostProcessRenderer> mPostProcess;
};