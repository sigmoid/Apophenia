#include "CheckBox.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../AudioBank.h"
#include "../Opal/Game.h"

CheckBox::CheckBox(std::string text, glm::vec4 bounds, std::shared_ptr<Opal::Renderer> renderer, bool is_checked, std::function<void(bool)> on_click)
{
    mText = text;
    mMesh = renderer->CreateMesh(20000);
    mCheckBoxMesh = renderer->CreateMesh(128);
    mCheckBoxMeshBG = renderer->CreateMesh(128);
    mBounds = bounds;
    mOnClick = on_click;
    mTimer = (rand() % 10000)/ 100000.0f;

    mIsChecked = is_checked;

    AudioBank::Instance->LoadClip("pluck1.wav");
    AudioBank::Instance->LoadClip("pluck2.wav");
    AudioBank::Instance->LoadClip("pluck3.wav");
    AudioBank::Instance->LoadClip("pluck4.wav");
    AudioBank::Instance->LoadClip("pluck5.wav");
}

void CheckBox::Tick(float dt)
{
    mTimer += dt;
    GenerateMesh();
    GenerateCheckboxMesh();

    #ifndef __IPHONEOS__
    glm::vec2 mousePos = Opal::InputHandler::GetMousePos();
#else
    glm::vec2 mousePos = Opal::InputHandler::GetTouchPos();
    mousePos.x *= 1920;
    mousePos.y *= 1080;
#endif

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
        (Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE) && !mLastSpacePressed) || 
        (Opal::InputHandler::GetTouch() && !mLastTouch))
        )
    {
        mIsChecked = !mIsChecked;
        mOnClick(mIsChecked);
    }
    mLastMouseButton = Opal::InputHandler::GetLeftMouseButtonDown();
    mLastControllerButton = Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_A);
    mLastSpacePressed = Opal::InputHandler::GetKey(SDL_SCANCODE_SPACE);
    mLastEnterPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_RETURN);
    mControllerInput = false;
    mLastTouch = Opal::InputHandler::GetTouch();

    if (!mIsHovering && !mCanPlaySound)
        mCanPlaySound = true;
}

void CheckBox::Render(std::shared_ptr<Opal::MeshRenderer2D> mesh_renderer, std::shared_ptr<Opal::FontRenderer> font_renderer)
{
    mesh_renderer->Submit(mMesh);
    mesh_renderer->Submit(mCheckBoxMeshBG);
    mesh_renderer->Submit(mCheckBoxMesh);

    float width = font_renderer->MeasureText(mText);
    float middleBoundsX = mBounds.x + (mBounds.z - mBounds.x)/2.0f;
    float middleBoundsY = mBounds.y + (mBounds.w - mBounds.y)/2.0f;

    font_renderer->RenderString(mText, middleBoundsX - width/2.0f - 75,
        middleBoundsY + font_renderer->GetSize() * 0.25f,
        mTextColor.r,
        mTextColor.g,
        mTextColor.b,
        mTextColor.a, 
        1.0f);

}

void CheckBox::Select(bool is_selected)
{
    mIsHovering = is_selected;
    mControllerInput = true;

    if (mIsHovering)
    {
        PlayToggleSound();
    }
}

void CheckBox::GenerateMesh()
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

void CheckBox::GenerateCheckboxMesh()
{
    mCheckBoxVertices.clear();

    int height = 50;
    int width = 50;
    int padding = 75;
    glm::vec2 topLeft = glm::vec2(mBounds.z - padding - width, (mBounds.y + mBounds.w) / 2 - height/2);
    glm::vec2 topRight = glm::vec2(topLeft.x + width, topLeft.y);
    glm::vec2 bottomLeft = glm::vec2(topLeft.x, topLeft.y + height);
    glm::vec2 bottomRight = glm::vec2(topLeft.x + width, topLeft.y + height);

    glm::vec4 color = mIsChecked ? mCheckedColor : mUnCheckedColor;

    mCheckBoxVertices.push_back(topLeft.x);
    mCheckBoxVertices.push_back(topLeft.y);
    mCheckBoxVertices.push_back(color.r);
    mCheckBoxVertices.push_back(color.g);
    mCheckBoxVertices.push_back(color.b);
    mCheckBoxVertices.push_back(color.a);

    mCheckBoxVertices.push_back(topRight.x);
    mCheckBoxVertices.push_back(topRight.y);
    mCheckBoxVertices.push_back(color.r);
    mCheckBoxVertices.push_back(color.g);
    mCheckBoxVertices.push_back(color.b);
    mCheckBoxVertices.push_back(color.a);

    mCheckBoxVertices.push_back(bottomLeft.x);
    mCheckBoxVertices.push_back(bottomLeft.y);
    mCheckBoxVertices.push_back(color.r);
    mCheckBoxVertices.push_back(color.g);
    mCheckBoxVertices.push_back(color.b);
    mCheckBoxVertices.push_back(color.a);

    mCheckBoxVertices.push_back(bottomLeft.x);
    mCheckBoxVertices.push_back(bottomLeft.y);
    mCheckBoxVertices.push_back(color.r);
    mCheckBoxVertices.push_back(color.g);
    mCheckBoxVertices.push_back(color.b);
    mCheckBoxVertices.push_back(color.a);

    mCheckBoxVertices.push_back(topRight.x);
    mCheckBoxVertices.push_back(topRight.y);
    mCheckBoxVertices.push_back(color.r);
    mCheckBoxVertices.push_back(color.g);
    mCheckBoxVertices.push_back(color.b);
    mCheckBoxVertices.push_back(color.a);

    mCheckBoxVertices.push_back(bottomRight.x);
    mCheckBoxVertices.push_back(bottomRight.y);
    mCheckBoxVertices.push_back(color.r);
    mCheckBoxVertices.push_back(color.g);
    mCheckBoxVertices.push_back(color.b);
    mCheckBoxVertices.push_back(color.a);

    mCheckBoxMesh->SetVertices(mCheckBoxVertices.data(), mCheckBoxVertices.size() * sizeof(float));

    mCheckBoxVerticesBG.clear();

    height = 60;
    width = 60;
    padding = 70;
    topLeft = glm::vec2(mBounds.z - padding - width, (mBounds.y + mBounds.w) / 2 - height / 2);
    topRight = glm::vec2(topLeft.x + width, topLeft.y);
    bottomLeft = glm::vec2(topLeft.x, topLeft.y + height);
    bottomRight = glm::vec2(topLeft.x + width, topLeft.y + height);

    color = mCheckBGColor;

    mCheckBoxVerticesBG.push_back(topLeft.x);
    mCheckBoxVerticesBG.push_back(topLeft.y);
    mCheckBoxVerticesBG.push_back(color.r);
    mCheckBoxVerticesBG.push_back(color.g);
    mCheckBoxVerticesBG.push_back(color.b);
    mCheckBoxVerticesBG.push_back(color.a);

    mCheckBoxVerticesBG.push_back(topRight.x);
    mCheckBoxVerticesBG.push_back(topRight.y);
    mCheckBoxVerticesBG.push_back(color.r);
    mCheckBoxVerticesBG.push_back(color.g);
    mCheckBoxVerticesBG.push_back(color.b);
    mCheckBoxVerticesBG.push_back(color.a);

    mCheckBoxVerticesBG.push_back(bottomLeft.x);
    mCheckBoxVerticesBG.push_back(bottomLeft.y);
    mCheckBoxVerticesBG.push_back(color.r);
    mCheckBoxVerticesBG.push_back(color.g);
    mCheckBoxVerticesBG.push_back(color.b);
    mCheckBoxVerticesBG.push_back(color.a);

    mCheckBoxVerticesBG.push_back(bottomLeft.x);
    mCheckBoxVerticesBG.push_back(bottomLeft.y);
    mCheckBoxVerticesBG.push_back(color.r);
    mCheckBoxVerticesBG.push_back(color.g);
    mCheckBoxVerticesBG.push_back(color.b);
    mCheckBoxVerticesBG.push_back(color.a);

    mCheckBoxVerticesBG.push_back(topRight.x);
    mCheckBoxVerticesBG.push_back(topRight.y);
    mCheckBoxVerticesBG.push_back(color.r);
    mCheckBoxVerticesBG.push_back(color.g);
    mCheckBoxVerticesBG.push_back(color.b);
    mCheckBoxVerticesBG.push_back(color.a);

    mCheckBoxVerticesBG.push_back(bottomRight.x);
    mCheckBoxVerticesBG.push_back(bottomRight.y);
    mCheckBoxVerticesBG.push_back(color.r);
    mCheckBoxVerticesBG.push_back(color.g);
    mCheckBoxVerticesBG.push_back(color.b);
    mCheckBoxVerticesBG.push_back(color.a);

    mCheckBoxMeshBG->SetVertices(mCheckBoxVerticesBG.data(), mCheckBoxVerticesBG.size() * sizeof(float));
}

void CheckBox::PlayToggleSound()
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