#include "MainMenuState.h"

#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/EntityComponent/SpriteComponent.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../Components/CursorComponent.h"
#include "../Components/NoiseMoveComponent.h"
#include "../Components/SentenceFragmentComponent.h"
#include "../Components/EndWallComponent.h"
#include "../Components/AttractableComponent.h"
#include "../Components/DragComponent.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/DialogueSerializer.h"
#include "../DialogueSystem/WordBank.h"
#include "../DialogueSystem/Response.h"

#include "../../Opal/Input/InputHandler.h"

#include "../../Opal/Logger.h"
#include "../GameSettings.h"

#include "EndState.h"
#include "CreditsState.h"
#include "../../Opal/Graphics/Vulkan/VulkanRenderer.h"

//std::shared_ptr<Opal::RenderPass> MainMenuState::mRenderPass = nullptr;

std::shared_ptr<Opal::Scene> MainMenuState::mScene = nullptr;
std::shared_ptr<Opal::BatchRenderer2D> MainMenuState::mBatch = nullptr;
std::shared_ptr<Opal::LineRenderer> MainMenuState::mLineRenderer = nullptr;
std::shared_ptr<Opal::MeshRenderer2D> MainMenuState::mMeshRenderer = nullptr;
std::shared_ptr<Opal::FontRenderer> MainMenuState::mFontRenderer = nullptr;
std::shared_ptr<Opal::Font> MainMenuState::mFont = nullptr;
std::shared_ptr<Opal::SpriteRenderer> MainMenuState::mSpriteRenderer = nullptr;
std::shared_ptr<Opal::Texture> MainMenuState::mTitleTexture = nullptr;

MainMenuState::MainMenuState()
{
    mPlayButton = std::make_shared<Button>("Play", glm::vec4(1920 / 2.0f - 150,
        1080 / 2.0f - 50,
        1920 / 2.0f + 150,
        1080 / 2.0f + 50), Opal::Game::Instance->Renderer, [=]() {mShouldPopState = true; });
    mOptionsButton = std::make_shared<Button>("Options", glm::vec4(1920 / 2.0f - 150,
        1080 / 2.0f - 50 + 125 * 1,
        1920 / 2.0f + 150,
        1080 / 2.0f + 50 + 125 * 1), Opal::Game::Instance->Renderer, [=]() {mCurrentState = MenuState::Options; mSwitchThisFrame = true; mSelectedButton = 0; });
    mCreditsButton = std::make_shared<Button>("Credits", glm::vec4(1920 / 2.0f - 150,
        1080 / 2.0f - 50 + 125 * 2,
        1920 / 2.0f + 150,
        1080 / 2.0f + 50 + 125 * 2), Opal::Game::Instance->Renderer, [=]() {Opal::Game::Instance->PushState<CreditsState>(); });
    mExitButton = std::make_shared<Button>("Exit", glm::vec4(1920 / 2.0f - 150,
        1080 / 2.0f - 50 + 125 * 3,
        1920 / 2.0f + 150,
        1080 / 2.0f + 50 + 125 * 3), Opal::Game::Instance->Renderer, []() {Opal::Game::Instance->End(); });

    mButtons = { mPlayButton, mOptionsButton, mCreditsButton, mExitButton };

    mVolumeSlider = std::make_shared<Slider>("Volume:", glm::vec4(1920 / 2.0f - 400,
        1080 / 2.0f - 40,
        1920 / 2.0f + 400,
        1080 / 2.0f + 40), Opal::Game::Instance->Renderer, GameSettings::GetMasterVolume(), [](float x) {GameSettings::SetMasterVolume(x); });
    mToggleAntialiasingCheckBox = std::make_shared<CheckBox>("Anti-Aliasing", glm::vec4(1920 / 2.0f - 400,
        1080 / 2.0f - 40 + 110 * 1,
        1920 / 2.0f + 400,
        1080 / 2.0f + 40 + 110 * 1), Opal::Game::Instance->Renderer,GameSettings::GetAntiAliasingEnabled(), [=](bool isChecked) {GameSettings::SetAntiAliasingEnabled(isChecked); });
    mExitOptionsButton = std::make_shared<Button>("Back", glm::vec4(1920 / 2.0f - 400,
        1080 / 2.0f - 40 + 110 * 2,
        1920 / 2.0f + 400,
        1080 / 2.0f + 40 + 110 * 2), Opal::Game::Instance->Renderer, [=]() {mCurrentState = MenuState::Default; mSwitchThisFrame = true; mSelectedButton = 0; });

    mOptionsButtons = { mVolumeSlider, mToggleAntialiasingCheckBox, mExitOptionsButton };
    mSelectedButton = -1;
}
MainMenuState::~MainMenuState()
{
    
}

void MainMenuState::Tick()
{
    mScene->Update(mGame->GetDeltaTime());

    if (abs(Opal::InputHandler::GetLeftJoystickY()) < 0.3f)
    {
        mJoystickReturned = true;
    }
    if ((mJoystickReturned && Opal::InputHandler::GetLeftJoystickY() > 0.5f) ||
        (Opal::InputHandler::GetKey(SDL_SCANCODE_S) && !mLastSPressed) ||
        (Opal::InputHandler::GetKey(SDL_SCANCODE_DOWN) && !mLastDownPressed))
    {
        auto buttonList = (mCurrentState == MenuState::Default) ? mButtons : mOptionsButtons;
            mSelectedButton++;

            if (mSelectedButton < 0)
                mSelectedButton = buttonList.size() - 1;

            mSelectedButton %= buttonList.size();

            mJoystickReturned = false;
        mUsingGamepad = true;
    }
    else if ((mJoystickReturned && Opal::InputHandler::GetLeftJoystickY() < -0.5f) ||
        (Opal::InputHandler::GetKey(SDL_SCANCODE_W) && !mLastWPressed)||
        (Opal::InputHandler::GetKey(SDL_SCANCODE_UP) && !mLastUpPressed))
    {
        auto buttonList = (mCurrentState == MenuState::Default) ? mButtons : mOptionsButtons;
        mSelectedButton--;

        if (mSelectedButton < 0)
            mSelectedButton = buttonList.size() - 1;

        mSelectedButton %= buttonList.size();

        mJoystickReturned = false;
        mUsingGamepad = true;
    }

    mLastDownPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_DOWN);
    mLastUpPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_UP);
    mLastWPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_W);
    mLastSPressed = Opal::InputHandler::GetKey(SDL_SCANCODE_S);

    if (!mSwitchThisFrame)
    {

        if (!mUsingGamepad)
        {
            mLastMousePosition = Opal::InputHandler::GetMousePos();

            switch (mCurrentState)
            {
            case MenuState::Default:
                mPlayButton->Tick(mGame->GetDeltaTime());
                mOptionsButton->Tick(mGame->GetDeltaTime());
                mCreditsButton->Tick(mGame->GetDeltaTime());
                mExitButton->Tick(mGame->GetDeltaTime());
                break;
            case MenuState::Options:
                mVolumeSlider->Tick(mGame->GetDeltaTime());
                mToggleAntialiasingCheckBox->Tick(mGame->GetDeltaTime());
                mExitOptionsButton->Tick(mGame->GetDeltaTime());
                break;
            }
        }
        else
        {
            if (glm::distance(mLastMousePosition, Opal::InputHandler::GetMousePos()) > 10.f || Opal::InputHandler::GetLeftMouseButtonDown())
            {
                mUsingGamepad = false;
            }
            else
            {
                switch (mCurrentState)
                {
                case MenuState::Default:
                    for (int i = 0; i < mButtons.size(); i++)
                    {
                        mButtons[i]->Select(i == mSelectedButton);
                        mButtons[i]->Tick(mGame->GetDeltaTime());
                    }
                    break;
                case MenuState::Options:
                    for (int i = 0; i < mOptionsButtons.size(); i++)
                    {
                        mOptionsButtons[i]->Select(i == mSelectedButton);
                        mOptionsButtons[i]->Tick(mGame->GetDeltaTime());
                    }
                    break;
                }

            }
        }
    }
    else
    {
        mSwitchThisFrame = false;
    }

    if(mShouldPopState)
        Opal::Game::Instance->PopState();
}

void MainMenuState::Render() 
{
    RenderSparks();

    mMeshRenderer->StartFrame();
    switch (mCurrentState)
    {
    case MenuState::Default:
        mPlayButton->Render(mMeshRenderer, mFontRenderer);
        mOptionsButton->Render(mMeshRenderer, mFontRenderer);
        mCreditsButton->Render(mMeshRenderer, mFontRenderer);
        mExitButton->Render(mMeshRenderer, mFontRenderer);
        break;
    case MenuState::Options:
        mVolumeSlider->Render(mMeshRenderer, mFontRenderer);
        mToggleAntialiasingCheckBox->Render(mMeshRenderer, mFontRenderer);
        mExitOptionsButton->Render(mMeshRenderer, mFontRenderer);
        break;
    }


    {
        std::string version = "Version 1.0.0";
        float padding = 25.f;
        float width = mFontRenderer->MeasureText(version);
        mFontRenderer->RenderString(version, 
            1920 - padding - width,
            1080 - padding - mFont->GetSize(),
            1.0f,
            1.0f,
            1.0f,
            0.7f,
            1.0f);
    }
    mBatch->StartBatch();
    mScene->Render(mBatch);
    mBatch->BatchSprite(mTitleSprite);
    mBatch->RenderBatch();

    mRenderPass->Record();
    mLineRenderer->Render();
    mBatch->RecordCommands();
    mMeshRenderer->RecordCommands();
    mFontRenderer->RecordCommands();
    mRenderPass->EndRecord();
    mGame->Renderer->SubmitRenderPass(mRenderPass);
}

void MainMenuState::Begin() 
{
    if(mRenderPass == nullptr)
    {
        mRenderPass = mGame->Renderer->CreateRenderPass(true);
        mRenderPass->SetClearColor(0.2f,0.2f,0.2f,1.0f);

        mFont = std::make_shared<Opal::Font>(mGame->Renderer,Opal::GetBaseContentPath().append("fonts/JosefinSans.ttf").c_str(), 64);
        mFontRenderer = mGame->Renderer->CreateFontRenderer(mRenderPass, *mFont, glm::vec2(1920, 1080), Opal::Camera::ActiveCamera);
        mMeshRenderer = mGame->Renderer->CreateMeshRenderer(mRenderPass);

        mTitleTexture = mGame->Renderer->CreateTexture(Opal::GetBaseContentPath().append("textures/Title.png"));
        mTitleSprite.SetTexture(mTitleTexture);
        float titleSpriteX = 1920 / 2.0f - mTitleTexture->GetWidth() /2.0f;
        float titleSpriteY = 50;
        mTitleSprite.SetPosition(titleSpriteX,titleSpriteY);

        std::vector<std::shared_ptr<Opal::Texture> > textures;
        textures.push_back(mTitleTexture);
        mBatch = mGame->Renderer->CreateBatch(mRenderPass, 1000, textures, true);

        mLineRenderer = std::make_shared<Opal::LineRenderer>();
        mLineRenderer->Init(mGame->Renderer, mRenderPass, true);

        mScene = std::make_shared<Opal::Scene>(mBatch);

        std::shared_ptr<Opal::Entity> dummyEntity = std::make_shared<Opal::Entity>();
        std::shared_ptr<Opal::TransformComponent> transform = std::make_shared<Opal::TransformComponent>();
        transform->Position = glm::vec3(0, 0, 0);
        dummyEntity->AddComponent(transform);
        std::shared_ptr<Opal::VelocityComponent> velocity = std::make_shared<Opal::VelocityComponent>();
        dummyEntity->AddComponent(velocity);
        
        mScene->AddEntity(dummyEntity);

        mScene->Start();
    }
    CreateSparks();
}

void MainMenuState::End() 
{

}

void MainMenuState::Resume() 
{
    mUsingGamepad = false;
    mSelectedButton = -1;
}

void MainMenuState::CreateSparks()
{
    for (int i = 0; i < mNumSparks; i++)
    {
        CreateRandomSpark();
    }
}

void MainMenuState::CreateRandomSpark()
{
    glm::vec2 pos = glm::vec2(rand() % 1920, rand() % 1080);
    glm::vec4 startColor = glm::vec4(1, 1, 1, 0.25f);
    glm::vec4 endColor = glm::vec4(1, 1, 1, 0);
    int length = rand() % 15 + 3;
    float res = 0.01f;
    int width = 2;
    float xVel = mLineSpeed * ((rand() % 100) / 100.0f * 0.5f + 0.9f);

    std::shared_ptr<Opal::Entity> mSparkEntity = std::make_shared<Opal::Entity>();

    std::shared_ptr<Opal::TransformComponent> transform = std::make_shared<Opal::TransformComponent>();
    transform->Position = glm::vec3(pos.x, pos.y, 0);
    mSparkEntity->AddComponent(transform);
    std::shared_ptr<Opal::VelocityComponent> velocity = std::make_shared<Opal::VelocityComponent>();
    velocity->SetVelocity(glm::vec3(xVel, 0, 0));
    mSparkEntity->AddComponent(velocity);
    std::shared_ptr<AttractableComponent> attr = std::make_shared<AttractableComponent>(true);
    mSparkEntity->AddComponent(attr);
    std::shared_ptr<SparkComponent> spark = std::make_shared<SparkComponent>(width, startColor, endColor, length, res);
    mSparkEntity->AddComponent(spark);
    std::shared_ptr<DragComponent> drag = std::make_shared<DragComponent>(1);
    mSparkEntity->AddComponent(drag);
    float scl = fmax(mLineSpeed - 800.0f, 0);
    scl /= 200.0f;
    scl *= mSparkScale;
    scl += 0.05f;

    float freq = fmax(mLineSpeed - 800.0f, 0);
    freq /= 200.0f;
    freq *= mSparkFreq;
    freq += 1;

    float str = fmax(mLineSpeed - 800.0f, 0);
    str /= 200.0f;
    str *= 4;
    str += 1;
    std::shared_ptr<NoiseMoveComponent> mover = std::make_shared<NoiseMoveComponent>(str, freq, scl, rand() % 10000);
    mSparkEntity->AddComponent(mover);

    mScene->AddEntity(mSparkEntity);
    mSparkEntities.push_back(mSparkEntity);
}

void MainMenuState::RenderSparks()
{
    std::vector<int> deleteIds;

    for (int i = 0; i < mSparkEntities.size(); i++)
    {
        std::shared_ptr<SparkComponent> spark = mSparkEntities[i]->GetComponent<SparkComponent>();
        //spark->SetSpeedUp((mSparkSpeedUp - 1) * ((float)i / (float)mSparkEntities.size()) + 1);
        std::shared_ptr<Opal::TransformComponent> trans = mSparkEntities[i]->GetComponent<Opal::TransformComponent>();

        if (trans->Position.x > 1920)
        {
            mScene->RemoveEntity(mSparkEntities[i]);
            deleteIds.push_back(i);
            CreateRandomSpark();
        }

        SparkTrail data = spark->TrailData;

        for (int j = data.Points.size() - 1; j > 1; j--)
        {
            glm::vec4 currentColor = data.StartColor;
            currentColor.r = Opal::OpalMath::Lerp(currentColor.r, data.EndColor.r, (float)(data.Points.size() - j) / (float)data.Points.size());
            currentColor.g = Opal::OpalMath::Lerp(currentColor.g, data.EndColor.g, (float)(data.Points.size() - j) / (float)data.Points.size());
            currentColor.b = Opal::OpalMath::Lerp(currentColor.b, data.EndColor.b, (float)(data.Points.size() - j) / (float)data.Points.size());
            currentColor.a = Opal::OpalMath::Lerp(currentColor.a, data.EndColor.a, (float)(data.Points.size() - j) / (float)data.Points.size());

            mLineRenderer->DrawLine(data.Points[j - 1], data.Points[j], currentColor, data.Width);
        }
    }

    std::reverse(deleteIds.begin(), deleteIds.end());

    for (int i = 0; i < deleteIds.size(); i++)
    {
        mSparkEntities.erase(mSparkEntities.begin() + deleteIds[i]);
    }
}

void MainMenuState::IncreaseVolume()
{
    auto volume = Opal::Game::Instance->mAudioEngine.GetGlobalVolume();
    if (volume >= 1.0f)
        return;
    GameSettings::SetMasterVolume(volume + 0.1f);
}

void MainMenuState::DecreaseVolume()
{
    auto volume = Opal::Game::Instance->mAudioEngine.GetGlobalVolume();
    if (volume <= 0.0f)
        return;
    GameSettings::SetMasterVolume(volume - 0.1f);
}


