#include "SentenceFormingState.h"

#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/EntityComponent/SpriteComponent.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../Components/CursorComponent.h"
#include "../Components/SentenceFragmentComponent.h"
#include "../Components/EndWallComponent.h"

SentenceFormingState::SentenceFormingState()
{

}

void SentenceFormingState::Tick() 
{
    mScene->Update(mGame->GetDeltaTime());
    UpdateCursorLine();
    if(!mCursorEntity->GetComponent<CursorComponent>()->GetAlive())
    {
        free(mCursorTexture);
        free(mScene);
        free(mBatch);
        free(mTextPass);
        free(mTextRenderer);
        free(mLineRenderer);
        mGame->PopState();
    }
}

void SentenceFormingState::Render() 
{
    mBatch->StartBatch();
    mScene->Render(mBatch);
    mBatch->RenderBatch();

    //mTextRenderer->RenderString("This is a response!", 1920/2 - 300, 1080/2, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    RenderSentenceFragments();

    mTextPass->Record();
    mBatch->RecordCommands();
    mTextRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);

    DrawCursorLine();

    mLineRenderer->Render();
}

void SentenceFormingState::Begin() 
{
    mTextPass = mGame->Renderer->CreateRenderPass(true);
    mTextPass->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Opal::Font typeFace(mGame->Renderer,"../fonts/JosefinSans.ttf", 90);

    mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, typeFace, glm::vec2(mGame->GetWidth(), mGame->GetHeight()), Opal::Camera::ActiveCamera);
    mLineRenderer = new Opal::LineRenderer();
    mLineRenderer->Init(mGame->Renderer);

    std::vector<Opal::Texture*> textures;
    mCursorTexture = mGame->Renderer->CreateTexture("../textures/cursor.png");
    textures.push_back(mCursorTexture);
    mBatch = mGame->Renderer->CreateBatch(mTextPass, 1000, textures, true);
    mScene = new Opal::Scene(mBatch);
    CreatePlayer();

    CreateSentenceFragment(glm::vec3(1920, 1080/2, 0), "I");

    CreateSentenceFragment(glm::vec3(1920 * 1.5f, 300, 0), "feel");
    CreateSentenceFragment(glm::vec3(1920 * 1.5f, 900, 0), "am");

    CreateSentenceFragment(glm::vec3(1920 * 2.f, 300, 0), "fine");
    CreateSentenceFragment(glm::vec3(1920 * 2.f, 900, 0), "great");

    CreateEndWall(1920 * 2.5f);

    mScene->Start();
}

void SentenceFormingState::End() 
{

}

void SentenceFormingState::Resume() 
{

}

void SentenceFormingState::CreateEndWall(float x)
{
    Opal::Entity *mEndWallEnt = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent( glm::vec3(x, 0, 0), glm::vec3(1,1,1), 0);
    mEndWallEnt->AddComponent(transform);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(60,2000), glm::vec2(0,0), true);
    collider->SetIsTrigger(true);
    collider->SetIsStatic(true);
    mEndWallEnt->AddComponent(collider);
    EndWallComponent *endComp = new EndWallComponent(mLineSpeed);
    mEndWallEnt->AddComponent(endComp);

    mScene->AddEntity(mEndWallEnt);
}

void SentenceFormingState::CreatePlayer()
{
    mCursorEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent( glm::vec3(1920/4, 1080/2, 0), glm::vec3(1,1,1), 0);
    mCursorEntity->AddComponent(transform);
    Opal::SpriteComponent *sprite = new Opal::SpriteComponent(mCursorTexture);
    mCursorEntity->AddComponent(sprite);
    CursorComponent *cursor = new CursorComponent();
    mCursorEntity->AddComponent(cursor);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(64,64), glm::vec2(0,0), false);
    mCursorEntity->AddComponent(collider);
    Opal::VelocityComponent *velocity = new Opal::VelocityComponent();
    mCursorEntity->AddComponent(velocity);

    mScene->AddEntity(mCursorEntity);
}


void SentenceFormingState::CreateSentenceFragment(glm::vec3 pos, std::string text)
{
    Opal::Entity *mFragmentEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent( pos, glm::vec3(1,1,1), 0);
    mFragmentEntity->AddComponent(transform);
    SentenceFragmentComponent *frag = new SentenceFragmentComponent(text,mLineSpeed, mFragmentColor);
    mFragmentEntity->AddComponent(frag);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(64,400), glm::vec2(0,-200), true);
    collider->SetIsTrigger(true);
    collider->SetIsStatic(true);
    mFragmentEntity->AddComponent(collider);

    mScene->AddEntity(mFragmentEntity);
}

void SentenceFormingState::RenderSentenceFragments()
{
    std::vector<Opal::Entity*> entities = mScene->GetAllEntities();

    for(int i = 0; i < entities.size(); i++)
    {
        SentenceFragmentComponent *frag = entities[i]->GetComponent<SentenceFragmentComponent>(); 
        if(frag != nullptr)
        {
            glm::vec3 pos = entities[i]->GetComponent<Opal::TransformComponent>()->Position;
            mTextRenderer->RenderString(frag->Text, pos.x, pos.y, frag->Color.r, frag->Color.g, frag->Color.b, frag->Color.a, 1.0f);
        }
    }
}

void SentenceFormingState::DrawCursorLine()
{
    if(mLinePoints.size() < 2)
    {
        return;
    }

    for(int i = 1; i < mLinePoints.size(); i++)
    {
        mLineRenderer->DrawLine(mLinePoints[i-1], mLinePoints[i], mLineColor, 3);
    }
}

void SentenceFormingState::UpdateCursorLine()
{
    int cutoffpoint = -1;
    for(int i = 0; i < mLinePoints.size(); i++)
    {
        mLinePoints[i].x -= mLineSpeed * mGame->GetDeltaTime();
        if(mLinePoints[i].x < mLineCutoff)
            cutoffpoint = i;
    }

    if(cutoffpoint != -1)
        mLinePoints.erase(mLinePoints.begin(), mLinePoints.begin()+cutoffpoint);

    mLineTimer -= mGame->GetDeltaTime();
    if(mLineTimer <= 0)
    {
        mLinePoints.push_back(glm::vec2(mCursorEntity->GetComponent<Opal::TransformComponent>()->Position.x + 32, mCursorEntity->GetComponent<Opal::TransformComponent>()->Position.y + 32));
        mLineTimer = mLineTimeStep;
    }
}