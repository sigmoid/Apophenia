#pragma once

#include "../../Opal/Gamestate/Gamestate.h"
#include "../../Opal/Graphics/Font.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Graphics/RenderPass.h"
#include "../../Opal/Graphics/Camera.h"

class EndState : public Opal::Gamestate
{

    public:
    EndState();
    virtual void Tick() override;
    virtual void Render() override;
    virtual void Begin() override;
    virtual void End() override;
    virtual void Resume() override;

    private:

    static Opal::FontRenderer *mTextRenderer;
    static Opal::RenderPass *mTextPass;
};