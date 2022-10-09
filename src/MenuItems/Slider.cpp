#include "Slider.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../AudioBank.h"
#include "../Opal/Game.h"

Slider::Slider(std::string text, glm::vec4 bounds, std::shared_ptr<Opal::Renderer> renderer, float value, std::function<void(float)> on_value_change, float sensitivity)
{
    mText = text;
    mBGMesh = renderer->CreateMesh(128);
    mCursorMesh = renderer->CreateMesh(512);
    mBounds = bounds;
    mOnValueChanged = on_value_change;
    mTimer = (rand() % 10000)/ 100000.0f;
    mSensitivity = sensitivity;

    mValue = value;

    AudioBank::Instance->LoadClip("pluck1.wav");
    AudioBank::Instance->LoadClip("pluck2.wav");
    AudioBank::Instance->LoadClip("pluck3.wav");
    AudioBank::Instance->LoadClip("pluck4.wav");
    AudioBank::Instance->LoadClip("pluck5.wav");
}

void Slider::Tick(float dt)
{
    mTimer += dt;
    GenerateBGMesh();
    GenerateCursorMesh();

    glm::vec2 mousePos = Opal::InputHandler::GetMousePos();

    if (abs(Opal::InputHandler::GetLeftJoystickX()) < 0.5f)
    {
        mJoystickReturned = true;
    }
    

    if (!mControllerInput)
    {
        mIsHovering = false;
        if (mousePos.x < mBounds.z && mousePos.x > mBounds.x &&
            mousePos.y < mBounds.w && mousePos.y > mBounds.y)
        {
            PlayToggleSound();
            mIsHovering = true;
        }
    }

    if(mIsHovering && (
        (Opal::InputHandler::GetKey(SDL_SCANCODE_RIGHT) && !mLastRightPressed)) ||
        (mJoystickReturned && Opal::InputHandler::GetLeftJoystickX() > 0.5f)
        )
    {
        mValue += mSensitivity;

        if (mValue > 1.0f)
            mValue = 1.0f;

        mOnValueChanged(mValue);

        mJoystickReturned = false;
    }

    if (mIsHovering && (
        (Opal::InputHandler::GetKey(SDL_SCANCODE_LEFT) && !mLastLeftPressed) ||
        (mJoystickReturned && Opal::InputHandler::GetLeftJoystickX() < -0.5f)
        ))
    {
        mValue -= mSensitivity;

        if (mValue < 0.0f)
            mValue = 0.0f;

        mOnValueChanged(mValue);
        mJoystickReturned = false;
    }

    if (mIsHovering && !mControllerInput && Opal::InputHandler::GetLeftMouseButtonDown())
    {
        auto mousePos = Opal::InputHandler::GetMousePos();
        
        mousePos -= (mBounds.z + mBounds.x) / 2;
        mousePos /= ((mBounds.z - mBounds.x)/ 2);

        if (mousePos.x < 0)
            mousePos.x = 0;
        else if (mousePos.x > 1)
            mousePos.x = 1;

        mValue = mousePos.x;
        mOnValueChanged(mValue);
    }
    mLastMouseButton = Opal::InputHandler::GetLeftMouseButtonDown();
    mLastLeftPressed = Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_A);
    mLastLeftPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_LEFT);
    mLastRightPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_RIGHT);
    mControllerInput = false;

    if (!mIsHovering && !mCanPlaySound)
        mCanPlaySound = true;
}

void Slider::Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer)
{
    mesh_renderer->Submit(mBGMesh);
    mesh_renderer->Submit(mCursorMesh);

    float width = font_renderer->MeasureText(mText);
    float middleBoundsX = mBounds.x + (mBounds.z - mBounds.x)/2.0f;
    float middleBoundsY = mBounds.y + (mBounds.w - mBounds.y)/2.0f;

    glm::vec4 color = (mIsHovering) ? mTextColorHover : mTextColor;

    font_renderer->RenderString(mText, mBounds.x + mTextLeftPosition,
        middleBoundsY + font_renderer->GetSize() * 0.25f,
        color.r,
        color.g,
        color.b,
        color.a, 
        1.0f);

}

void Slider::Select(bool is_selected)
{
    mIsHovering = is_selected;
    mControllerInput = true;

    if (mIsHovering)
    {
        PlayToggleSound();
    }
}

void Slider::GenerateBGMesh()
{
    mBGMeshVertices.clear();

    int height = 10;
    int width = (mBounds.z - mBounds.x) / 2.0f;

    glm::vec2 topLeft = glm::vec2((mBounds.x + mBounds.z) / 2.0f, (mBounds.y + mBounds.w) / 2.0f - height / 2.0f);
    glm::vec2 topRight = glm::vec2(topLeft.x + width, topLeft.y);
    glm::vec2 bottomLeft = glm::vec2(topLeft.x, topLeft.y + height);
    glm::vec2 bottomRight = glm::vec2(topLeft.x + width, topLeft.y + height);

    glm::vec4 color = mBGColor;

    // top left
    mBGMeshVertices.push_back(topLeft.x);
    mBGMeshVertices.push_back(topLeft.y);
    mBGMeshVertices.push_back(color.r);
    mBGMeshVertices.push_back(color.g);
    mBGMeshVertices.push_back(color.b);
    mBGMeshVertices.push_back(color.a);

    // top right
    mBGMeshVertices.push_back(topRight.x);
    mBGMeshVertices.push_back(topRight.y);
    mBGMeshVertices.push_back(color.r);
    mBGMeshVertices.push_back(color.g);
    mBGMeshVertices.push_back(color.b);
    mBGMeshVertices.push_back(color.a);

    // bottom left
    mBGMeshVertices.push_back(bottomLeft.x);
    mBGMeshVertices.push_back(bottomLeft.y);
    mBGMeshVertices.push_back(color.r);
    mBGMeshVertices.push_back(color.g);
    mBGMeshVertices.push_back(color.b);
    mBGMeshVertices.push_back(color.a);

    // bottom left
    mBGMeshVertices.push_back(bottomLeft.x);
    mBGMeshVertices.push_back(bottomLeft.y);
    mBGMeshVertices.push_back(color.r);
    mBGMeshVertices.push_back(color.g);
    mBGMeshVertices.push_back(color.b);
    mBGMeshVertices.push_back(color.a);

    // top right
    mBGMeshVertices.push_back(topRight.x);
    mBGMeshVertices.push_back(topRight.y);
    mBGMeshVertices.push_back(color.r);
    mBGMeshVertices.push_back(color.g);
    mBGMeshVertices.push_back(color.b);
    mBGMeshVertices.push_back(color.a);

    // bottom right
    mBGMeshVertices.push_back(bottomRight.x);
    mBGMeshVertices.push_back(bottomRight.y);
    mBGMeshVertices.push_back(color.r);
    mBGMeshVertices.push_back(color.g);
    mBGMeshVertices.push_back(color.b);
    mBGMeshVertices.push_back(color.a);

    mBGMesh->SetVertices(mBGMeshVertices.data(), mBGMeshVertices.size() * sizeof(float));
}

void Slider::GenerateCursorMesh()
{
    mCursorVertices.clear();

    int height = 50;
    int BGWidth = (mBounds.z - mBounds.x) / 2.0f;
    int width = 20;

    glm::vec2 topLeft = glm::vec2((mBounds.x + mBounds.z) / 2.0f + BGWidth * mValue, (mBounds.y + mBounds.w) / 2.0f - height / 2.0f);
    glm::vec2 topRight = glm::vec2(topLeft.x + width, topLeft.y);
    glm::vec2 bottomLeft = glm::vec2(topLeft.x, topLeft.y + height);
    glm::vec2 bottomRight = glm::vec2(topLeft.x + width, topLeft.y + height);

    glm::vec4 color = (mIsHovering) ? mMeshHoverColor : mBGColor;

    // top left
    mCursorVertices.push_back(topLeft.x);
    mCursorVertices.push_back(topLeft.y);
    mCursorVertices.push_back(color.r);
    mCursorVertices.push_back(color.g);
    mCursorVertices.push_back(color.b);
    mCursorVertices.push_back(color.a);

    // top right
    mCursorVertices.push_back(topRight.x);
    mCursorVertices.push_back(topRight.y);
    mCursorVertices.push_back(color.r);
    mCursorVertices.push_back(color.g);
    mCursorVertices.push_back(color.b);
    mCursorVertices.push_back(color.a);

    // bottom left
    mCursorVertices.push_back(bottomLeft.x);
    mCursorVertices.push_back(bottomLeft.y);
    mCursorVertices.push_back(color.r);
    mCursorVertices.push_back(color.g);
    mCursorVertices.push_back(color.b);
    mCursorVertices.push_back(color.a);

    // bottom left
    mCursorVertices.push_back(bottomLeft.x);
    mCursorVertices.push_back(bottomLeft.y);
    mCursorVertices.push_back(color.r);
    mCursorVertices.push_back(color.g);
    mCursorVertices.push_back(color.b);
    mCursorVertices.push_back(color.a);

    // top right
    mCursorVertices.push_back(topRight.x);
    mCursorVertices.push_back(topRight.y);
    mCursorVertices.push_back(color.r);
    mCursorVertices.push_back(color.g);
    mCursorVertices.push_back(color.b);
    mCursorVertices.push_back(color.a);

    // bottom right
    mCursorVertices.push_back(bottomRight.x);
    mCursorVertices.push_back(bottomRight.y);
    mCursorVertices.push_back(color.r);
    mCursorVertices.push_back(color.g);
    mCursorVertices.push_back(color.b);
    mCursorVertices.push_back(color.a);

    mCursorMesh->SetVertices(mCursorVertices.data(), mCursorVertices.size() * sizeof(float));
}

void Slider::PlayToggleSound()
{
    if (!mCanPlaySound)
        return;

    int pluck = (rand() % 5) + 1;
    std::string filename = "pluck";
    filename.append(std::to_string(pluck)).append(".wav");
    auto clip = AudioBank::Instance->GetClip(filename);
    Opal::Game::Instance->mAudioEngine.PlaySound(clip, 0.3f, 1.0f, 0.0f, false);

    mCanPlaySound = false;
}