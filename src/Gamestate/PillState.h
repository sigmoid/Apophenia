#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"
#include "../../Opal/Graphics/MeshRenderer2D.h"
#include "../../Opal/Graphics/RenderPass.h"

#include "../PillGeneration/Pill.h"

class PillState : public Opal::Gamestate
{

    public:
    PillState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    static int NumPills;
    static float Duration;

    private:
    std::vector<Pill> mPills;

    float mTimer;

    static Opal::MeshRenderer2D *mMeshRenderer;
    static Opal::RenderPass *mRenderPass;
};