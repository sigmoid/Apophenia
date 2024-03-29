#include "Button.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../AudioBank.h"
#include "../Opal/Game.h"

Button::Button(std::string text, glm::vec4 bounds, std::shared_ptr<Opal::Renderer> renderer, std::function<void()> on_click)
{
    mText = text;
    mMesh = renderer->CreateMesh(20000);
    mBounds = bounds;
    mOnClick = on_click;
    mTimer = (rand() % 10000)/ 100000.0f;

    AudioBank::Instance->LoadClip("pluck1.wav");
    AudioBank::Instance->LoadClip("pluck2.wav");
    AudioBank::Instance->LoadClip("pluck3.wav");
    AudioBank::Instance->LoadClip("pluck4.wav");
    AudioBank::Instance->LoadClip("pluck5.wav");
}

void Button::Tick(float dt)
{
    mTimer += dt;
    GenerateMesh();

    glm::vec2 mousePos = Opal::InputHandler::GetMousePos();

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

    if(mIsHovering && ((Opal::InputHandler::GetLeftMouseButtonDown() && !mLastMouseButton) || 
        (Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_A) && !mLastControllerButton) ||
        (Opal::InputHandler::GetKey(SDL_SCANCODE_RETURN) && !mLastEnterPressed) ||
        (Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE) && !mLastSpacePressed))
        )
    {
        mOnClick();
    }
    mLastMouseButton = Opal::InputHandler::GetLeftMouseButtonDown();
    mLastControllerButton = Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_A);
    mLastSpacePressed = Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE);
    mLastEnterPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_RETURN);
    mControllerInput = false;

    if (!mIsHovering && !mCanPlaySound)
        mCanPlaySound = true;
}

void Button::Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer)
{
    mesh_renderer->Submit(mMesh);

    float width = font_renderer->MeasureText(mText);
    float middleBoundsX = mBounds.x + (mBounds.z - mBounds.x)/2.0f;
    float middleBoundsY = mBounds.y + (mBounds.w - mBounds.y)/2.0f;

    font_renderer->RenderString(mText, middleBoundsX - width/2.0f,
        middleBoundsY + font_renderer->GetSize() * 0.25f,
        mTextColor.r,
        mTextColor.g,
        mTextColor.b,
        mTextColor.a, 
        1.0f);

}

void Button::Select(bool is_selected)
{
    mIsHovering = is_selected;
    mControllerInput = true;

    if (mIsHovering)
    {
        PlayToggleSound();
    }
}

void Button::GenerateMesh()
{
    float noiseSeed = mTimer;

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(mNoiseFreq);
    mVertices.clear();

    glm::vec4 color = (mIsHovering) ? mMeshHoverColor : mMeshColor;
    glm::vec2 center = glm::vec2((mBounds.x + mBounds.z) / 2.0f,
        (mBounds.y + mBounds.w)/ 2.0f);

    glm::vec2 lastPos = glm::vec2(mBounds.x, mBounds.y);

    int numPoints = (int)((mBounds.z - mBounds.x) / mResolution);
    float dist = (mBounds.z - mBounds.x) / (float)numPoints;

    int noiseNum = 0;

    for(int i = 0; i < numPoints; i ++)
    {
        noiseNum++;
        float noiseVal = noise.GetNoise<float>((noiseNum * mNoiseScale),noiseSeed) * mNoiseIntensity;
        glm::vec2 newPos = glm::vec2(lastPos.x + dist, lastPos.y);
        
        float amp = 1.0f - abs((float)(numPoints / 2.0f) - i) /  (numPoints / 2.0f);
        if(i == 0 || i == numPoints - 1)
            amp = 0;
        newPos.y  = mBounds.y + noiseVal * amp;

        // top left
        mVertices.push_back(lastPos.x);
        mVertices.push_back(lastPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // top right
        mVertices.push_back(newPos.x);
        mVertices.push_back(newPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // center
        mVertices.push_back(center.x);
        mVertices.push_back(center.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);

        lastPos = newPos;
    }

    lastPos = glm::vec2(mBounds.z, mBounds.y);
    numPoints = (int)((mBounds.w - mBounds.y) / mResolution);
    dist = (mBounds.w - mBounds.y) / (float)numPoints;

    for(int i = 0; i < numPoints; i ++)
    {
        noiseNum++;
        float noiseVal = noise.GetNoise<float>((noiseNum * mNoiseScale),noiseSeed) * mNoiseIntensity;
        glm::vec2 newPos = glm::vec2(lastPos.x, lastPos.y + dist);
        
        float amp = 1.0f - abs((float)(numPoints / 2.0f) - i) /  (numPoints / 2.0f);
        if(i == 0 || i == numPoints - 1)
            amp = 0;
        newPos.x  = mBounds.z + noiseVal * amp;

        // top left
        mVertices.push_back(lastPos.x);
        mVertices.push_back(lastPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // top right
        mVertices.push_back(newPos.x);
        mVertices.push_back(newPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // center
        mVertices.push_back(center.x);
        mVertices.push_back(center.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);

        lastPos = newPos;
    }

    lastPos = glm::vec2(mBounds.z, mBounds.w);
    numPoints = (int)((mBounds.z - mBounds.x) / mResolution);
    dist = (mBounds.z - mBounds.x) / (float)numPoints;

    for(int i = 0; i < numPoints; i ++)
    {
        noiseNum++;
        float noiseVal = noise.GetNoise<float>((noiseNum * mNoiseScale),noiseSeed) * mNoiseIntensity;
        glm::vec2 newPos = glm::vec2(lastPos.x - dist, lastPos.y);
        
        float amp = 1.0f - abs((float)(numPoints / 2.0f) - i) /  (numPoints / 2.0f);;
        if(i == 0 || i == numPoints - 1)
            amp = 0;
        newPos.y  = mBounds.w + noiseVal * amp;

        // top left
        mVertices.push_back(lastPos.x);
        mVertices.push_back(lastPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // top right
        mVertices.push_back(newPos.x);
        mVertices.push_back(newPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // center
        mVertices.push_back(center.x);
        mVertices.push_back(center.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);

        lastPos = newPos;
    }

    lastPos = glm::vec2(mBounds.x, mBounds.w);
    numPoints = (int)((mBounds.w - mBounds.y) / mResolution);
    dist = (mBounds.w - mBounds.y) / (float)numPoints;

    for(int i = 0; i < numPoints; i ++)
    {
        noiseNum++;
        float noiseVal = noise.GetNoise<float>((noiseNum * mNoiseScale),noiseSeed) * mNoiseIntensity;
        glm::vec2 newPos = glm::vec2(lastPos.x, lastPos.y - dist);
        
        float amp = 1.0f - abs((float)(numPoints / 2.0f) - i) /  (numPoints / 2.0f);
        if(i == 0 || i == numPoints - 1)
            amp = 0;
        newPos.x  = mBounds.x + noiseVal * amp;

        // top left
        mVertices.push_back(lastPos.x);
        mVertices.push_back(lastPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // top right
        mVertices.push_back(newPos.x);
        mVertices.push_back(newPos.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);
        // center
        mVertices.push_back(center.x);
        mVertices.push_back(center.y);
        mVertices.push_back(color.r);
        mVertices.push_back(color.g);
        mVertices.push_back(color.b);
        mVertices.push_back(color.a);

        lastPos = newPos;
    }

    mMesh->SetVertices(mVertices.data(), mVertices.size() * sizeof(float));
}

void Button::PlayToggleSound()
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