#pragma once

#include <string>
#include <functional>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include "../../Opal/Graphics/MeshRenderer2D.h"
#include "../../Opal/Graphics/Mesh2D.h"
#include "../../Opal/Graphics/FontRenderer.h"
#include "../../Opal/Input/InputHandler.h"
#include "UIElement.h"

class Button : public UIElement
{
    public:
    Button(std::string text, glm::vec4 bounds, std::shared_ptr<Opal::Renderer> renderer, std::function<void()> on_click);
    virtual void Tick(float dt) override;
    virtual void Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer) override;
    virtual void Select(bool is_selected) override;

    private:
    std::function<void()> mOnClick;
    std::string mText;
    std::shared_ptr<Opal::Mesh2D> mMesh;
    std::shared_ptr<Opal::Renderer> mRenderer;
    glm::vec4 mBounds;
    glm::vec4 mTextColor = glm::vec4(0.2f,0.2f,0.2f,1.0f);

    bool mIsHovering = false;

    void GenerateMesh();
    std::vector<float> mVertices;
    glm::vec4 mMeshColor = glm::vec4(0.9f,0.9f,0.9f, 0.6f);
    glm::vec4 mMeshHoverColor = glm::vec4(0.9f,0.9f,0.9f, 1.0f);

    float mResolution = 5.0f;

    float mNoiseScale = 0.05, mNoiseFreq = 2;
    float mNoiseIntensity = 10;

    float mTimer = 0;
    bool mControllerInput = false;

    bool mLastMouseButton = true;
    bool mLastControllerButton = false;
    bool mLastEnterPressed = true, mLastSpacePressed = true;

    bool mCanPlaySound = true;
    void PlayToggleSound();
};