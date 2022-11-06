#include "LogoState.h"

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

//std::shared_ptr<Opal::RenderPass> LogoState::mRenderPass = nullptr;

std::shared_ptr<Opal::Scene> LogoState::mScene = nullptr;
std::shared_ptr<Opal::BatchRenderer2D> LogoState::mBatch = nullptr;
std::shared_ptr<Opal::LineRenderer> LogoState::mLineRenderer = nullptr;
std::shared_ptr<Opal::SpriteRenderer> LogoState::mSpriteRenderer = nullptr;
std::shared_ptr<Opal::Texture> LogoState::mLogoTexture = nullptr;

LogoState::LogoState() 
{
}
LogoState::~LogoState()
{

}

void LogoState::Tick()
{
    mScene->Update(mGame->GetDeltaTime());

    mTimer -= mGame->GetDeltaTime();

    if (mTimer <= 0 || Opal::InputHandler::GetTouch())
        mShouldPopState = true;

    if (mShouldPopState)
        Opal::Game::Instance->PopState();
}

void LogoState::Render()
{
    RenderSparks();


    mBatch->StartBatch();
    mScene->Render(mBatch);
    mBatch->BatchSprite(mLogoSprite);
    mBatch->RenderBatch();

    mRenderPass->Record();
    mLineRenderer->Render();
    mBatch->RecordCommands();
    mRenderPass->EndRecord();
    mGame->Renderer->SubmitRenderPass(mRenderPass);
}

void LogoState::Begin()
{
    if (mRenderPass == nullptr)
    {
        mRenderPass = mGame->Renderer->CreateRenderPass(true);
        mRenderPass->SetClearColor(0.3f, 0.3f, 0.3f, 1.0f);

        mLogoTexture = mGame->Renderer->CreateTexture(Opal::GetBaseContentPath().append("textures/Logo.png"));
        mLogoSprite.SetTexture(mLogoTexture);
        float titleSpriteX = mGame->GetWidth() / 2.0f - mLogoTexture->GetWidth() / 2.0f;
        float titleSpriteY = 0;
        mLogoSprite.SetPosition(titleSpriteX, titleSpriteY);

        std::vector<std::shared_ptr<Opal::Texture> > textures;
        textures.push_back(mLogoTexture);
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

void LogoState::End()
{

}

void LogoState::Resume()
{
}

void LogoState::CreateSparks()
{
    for (int i = 0; i < mNumSparks; i++)
    {
        CreateRandomSpark();
    }
}

void LogoState::CreateRandomSpark()
{
    glm::vec2 pos = glm::vec2(rand() % mGame->GetWidth(), rand() % mGame->GetHeight());
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

void LogoState::RenderSparks()
{
    std::vector<int> deleteIds;

    for (int i = 0; i < mSparkEntities.size(); i++)
    {
        std::shared_ptr<SparkComponent> spark = mSparkEntities[i]->GetComponent<SparkComponent>();
        //spark->SetSpeedUp((mSparkSpeedUp - 1) * ((float)i / (float)mSparkEntities.size()) + 1);
        std::shared_ptr<Opal::TransformComponent> trans = mSparkEntities[i]->GetComponent<Opal::TransformComponent>();

        if (trans->Position.x > mGame->GetWidth())
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


