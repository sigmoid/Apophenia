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

class Slider : public UIElement
{
    public:
    Slider(std::string text, glm::vec4 bounds, std::shared_ptr<Opal::Renderer> renderer, float value, std::function<void(float)> on_value_change, float sensitivity = 0.1f);
    virtual void Tick(float dt) override;
    virtual void Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer) override;
    virtual void Select(bool is_selected) override;

    private:
    std::function<void(float)> mOnValueChanged;
    std::string mText;
    std::shared_ptr<Opal::Mesh2D> mBGMesh;
    std::shared_ptr<Opal::Mesh2D> mCursorMesh;
    std::shared_ptr<Opal::Renderer> mRenderer;
    glm::vec4 mBounds;
    glm::vec4 mTextColor = glm::vec4(0.8f,0.8f,0.8f,1.0f);
    glm::vec4 mTextColorHover = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    bool mIsHovering = false;

    void GenerateBGMesh();
    void GenerateCursorMesh();
    std::vector<float> mBGMeshVertices;
    std::vector<float> mCursorVertices, mCheckBoxVerticesBG;
    glm::vec4 mMeshColor = glm::vec4(0.9f,0.9f,0.9f, 0.6f);
    glm::vec4 mMeshHoverColor = glm::vec4(0.9f,0.9f,0.9f, 1.0f);

    glm::vec4 mCursorColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 mBGColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    bool mJoystickReturned = false;

    float mValue;

    float mResolution = 5.0f;

    float mNoiseScale = 0.05, mNoiseFreq = 2;
    float mNoiseIntensity = 10;

    float mTimer = 0;
    bool mControllerInput = false;

    bool mLastMouseButton = true;
    bool mLastRightPressed = true, mLastLeftPressed = true;

    float mSensitivity = 0.1f;
    float mTextLeftPosition = 25.0f;

    bool mCanPlaySound = true;
    void PlayToggleSound();
};