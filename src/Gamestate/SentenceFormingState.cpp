#include "SentenceFormingState.h"

#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/EntityComponent/SpriteComponent.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../Components/CursorComponent.h"
#include "../Components/NoiseMoveComponent.h"
#include "../Components/SentenceFragmentComponent.h"
#include "../Components/EndWallComponent.h"
#include "../Components/AttractableComponent.h"
#include "../Components/DragComponent.h"
#include "../DialogueSystem/DialogueManager.h"
#include "../DialogueSystem/Response.h"

// Static members
Opal::FontRenderer *SentenceFormingState::mTextRenderer = nullptr;
Opal::RenderPass *SentenceFormingState::mTextPass = nullptr;
Opal::BatchRenderer2D *SentenceFormingState::mBatch = nullptr;
Opal::MeshRenderer2D *SentenceFormingState::mMeshRenderer = nullptr;
Opal::Texture *SentenceFormingState::mCursorTexture = nullptr;
Opal::LineRenderer *SentenceFormingState::mLineRenderer = nullptr;
Opal::Font *SentenceFormingState::mFont = nullptr;


SentenceFormingState::SentenceFormingState()
{

}

void SentenceFormingState::Tick() 
{
    if(mScreenShakeTimer > 0)
    {
        Opal::Camera::ActiveCamera->MoveCamera(glm::vec2((rand() % mScreenShakeIntensity * 1000) / 1000.0f - (float)mScreenShakeIntensity/2, (rand() % mScreenShakeIntensity * 1000) / 1000.0f - (float)mScreenShakeIntensity/2));
        mScreenShakeTimer -= mGame->GetDeltaTime();

        if(mScreenShakeTimer <= 0)
        {
            Opal::Camera::ActiveCamera->MoveCamera(glm::vec2(0,0));
        }
    }
    mScene->Update(mGame->GetDeltaTime());
    UpdateCursorLine();
    if(!mCursorEntity->GetComponent<CursorComponent>()->GetAlive())
    {
        auto response = mCursorEntity->GetComponent<CursorComponent>()->GetResponse();
        std::string responseStr = ConcatSelection(response);
        if(!DialogueManager::Instance->ProcessResponse(responseStr))
        {
            for(int i = 0; i < mFragmentEnts.size();i++)
            {
                mScene->RemoveEntity(mFragmentEnts[i]);
            }
            mFragmentEnts.clear();
            mScene->RemoveEntity(mEndWall);

            mCursorEntity->GetComponent<CursorComponent>()->Reset();

            CreatePlayingField();

            StartScreenShake();
        }
        else
        {
            free(mScene);
            mGame->PopState();
        }
    }
}

void SentenceFormingState::Render() 
{
    mBatch->StartBatch();
    mScene->Render(mBatch);
    mBatch->RenderBatch();


    mMeshRenderer->StartFrame();
    mMeshRenderer->Submit(mCursorEntity->GetComponent<CursorComponent>()->GetMesh());

    //mTextRenderer->RenderString("This is a response!", 1920/2 - 300, 1080/2, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    RenderSentenceFragments();
    RenderCurrentSelection();

    mTextPass->Record();
    mBatch->RecordCommands();
    mTextRenderer->RecordCommands();
    mMeshRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);

    // dEBUG mLineRenderer->DrawLine(glm::vec2(0,0), glm::vec2(1000,1000), glm::vec4(1,0,0,1), 3);
    DrawCursorLine();
    RenderSparks();

    mLineRenderer->Render();
}

void SentenceFormingState::Begin() 
{
    if(mTextRenderer == nullptr)
    {
        mTextPass = mGame->Renderer->CreateRenderPass(true);
        mTextPass->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        mFont = new Opal::Font(mGame->Renderer,"../fonts/JosefinSans-Light.ttf", 90);

        mMeshRenderer = mGame->Renderer->CreateMeshRenderer(mTextPass);

        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mFont, glm::vec2(mGame->GetWidth() - 200, mGame->GetHeight()), Opal::Camera::ActiveCamera);
        mLineRenderer = new Opal::LineRenderer();
        mLineRenderer->Init(mGame->Renderer, true);

        std::vector<Opal::Texture*> textures;
        mCursorTexture = mGame->Renderer->CreateTexture("../textures/cursor.png");
        textures.push_back(mCursorTexture);
        mBatch = mGame->Renderer->CreateBatch(mTextPass, 1000, textures, true);
    }
    mScene = new Opal::Scene(mBatch);
    CreatePlayer();
    CreateSparks();
    PreBakeLines();

    CreatePlayingField();
}

void SentenceFormingState::CreatePlayingField()
{
    Response resp = DialogueManager::Instance->GetCurrentResponse();  

    float start = 1920;
    float inc = 1920 * .7f;

    mLineSpeed = resp.Speed;
    for(int i = 0; i < resp.Fragments.size(); i++)
    {
        std::vector<Opal::AABB> yPositions;
        for(int j = 0; j < resp.Fragments[i].size(); j++)
        {
            int padding = 200;
            float ypos;
            bool needNew = true;
            while(needNew)
            {
                needNew = false;
                ypos = rand() % (1080 - padding*2) + padding; 
                Opal::AABB thisBox;
                thisBox.min = glm::vec2(start+inc*i, ypos - mFragmentSize/2);
                thisBox.max = glm::vec2(start+inc*i + 10, ypos+ mFragmentSize/2);
                for(int k = 0; k < yPositions.size(); k++)
                {
                    if(Opal::AABBCollision::GetResolution(thisBox,yPositions[k]) != glm::vec2(0,0))
                    {
                        needNew = true;
                    }
                }
            }
            CreateSentenceFragment(glm::vec3(start + inc * i, ypos, 0), resp.Fragments[i][j].Text, resp.Fragments[i][j].Attraction, resp.Speed);
            Opal::AABB newBox;
            newBox.min = glm::vec2(start+inc*i, ypos-mFragmentSize/2);
            newBox.max = glm::vec2(start+inc*i + 10, ypos + mFragmentSize/2);
            yPositions.push_back(newBox);
        }
    }

    CreateEndWall(start + inc * (resp.Fragments.size()));
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
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(600,2000), glm::vec2(0,0), true);
    collider->SetIsTrigger(true);
    collider->SetIsStatic(true);
    mEndWallEnt->AddComponent(collider);
    EndWallComponent *endComp = new EndWallComponent(mLineSpeed);
    mEndWallEnt->AddComponent(endComp);

    mScene->AddEntity(mEndWallEnt);
    mEndWall = mEndWallEnt;
}

void SentenceFormingState::CreatePlayer()
{
    mCursorEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent( glm::vec3(1920/4, 1080/2, 0), glm::vec3(1,1,1), 0);
    mCursorEntity->AddComponent(transform);
    //Opal::SpriteComponent *sprite = new Opal::SpriteComponent(mCursorTexture);
    //mCursorEntity->AddComponent(sprite);
    CursorComponent *cursor = new CursorComponent();
    mCursorEntity->AddComponent(cursor);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(64,64), glm::vec2(-32,-32), false);
    mCursorEntity->AddComponent(collider);
    Opal::VelocityComponent *velocity = new Opal::VelocityComponent();
    mCursorEntity->AddComponent(velocity);
    AttractableComponent *attr = new AttractableComponent(true);
    mCursorEntity->AddComponent(attr);
    DragComponent *drag = new DragComponent(1);
    mCursorEntity->AddComponent(drag);

    mScene->AddEntity(mCursorEntity);
}


void SentenceFormingState::CreateSentenceFragment(glm::vec3 pos, std::string text, float attraction, float speed)
{
    //Kludge, haven't actually implemented text measuring yet
    float width = mTextRenderer->MeasureText(text);

    Opal::Entity *mFragmentEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent( pos, glm::vec3(1,1,1), 0);
    mFragmentEntity->AddComponent(transform);
    SentenceFragmentComponent *frag = new SentenceFragmentComponent(text,speed, mFragmentColor, attraction);
    mFragmentEntity->AddComponent(frag);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(fmax(64, width),mFragmentSize), glm::vec2(0,-mFragmentSize), true);
    collider->SetIsTrigger(true);
    collider->SetIsStatic(true);
    mFragmentEntity->AddComponent(collider);

    mScene->AddEntity(mFragmentEntity);
    mFragmentEnts.push_back(mFragmentEntity);
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
            if(frag->Attraction > 0)
            {
                pos += glm::vec3((rand() % 1000) / 100 - 5, (rand() % 1000) / 100 - 5, 0) * (frag->Attraction / 600);
            }
            mTextRenderer->RenderString(frag->Text, pos.x, pos.y, frag->Color.r, frag->Color.g, frag->Color.b, frag->Color.a, 1.0f);

            // Draw colliders
            //mLineRenderer->DrawRect(entities[i]->GetComponent<Opal::BoxColliderComponent2D>()->GetAABB().min, entities[i]->GetComponent<Opal::BoxColliderComponent2D>()->GetAABB().max, glm::vec4(0,1,0,1), 2);
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

void SentenceFormingState::UpdateCursorLine(float timeOverride)
{
    int cutoffpoint = -1;
    for(int i = 0; i < mLinePoints.size(); i++)
    {
        mLinePoints[i].x -= mLineSpeed * ((timeOverride > 0) ? timeOverride : mGame->GetDeltaTime());
        if(mLinePoints[i].x < mLineCutoff)
            cutoffpoint = i;
    }

    if(cutoffpoint != -1)
    {
        mLinePoints.erase(mLinePoints.begin(), mLinePoints.begin()+cutoffpoint);
    }
    mLineTimer -= mGame->GetDeltaTime();
    if(mLineTimer <= 0)
    {
        mLinePoints.push_back(glm::vec2(mCursorEntity->GetComponent<Opal::TransformComponent>()->Position.x, mCursorEntity->GetComponent<Opal::TransformComponent>()->Position.y));
        mLineTimer = mLineTimeStep;
    }
}

void SentenceFormingState::StartScreenShake()
{
    mScreenShakeTimer = mScreenShakeTime;
}

void SentenceFormingState::RenderCurrentSelection()
{
    std::string resp = ConcatSelection(mCursorEntity->GetComponent<CursorComponent>()->GetResponse());
    mTextRenderer->RenderString(resp, 200, 1080-150, mFragmentColor.r, mFragmentColor.g, mFragmentColor.b, mFragmentColor.a, 1.0f);
}

std::string SentenceFormingState::ConcatSelection(std::vector<std::string> selection)
{
    std::string res = "";
    for(int i = 0; i < selection.size(); i++)
    {
        res += selection[i] + ((i == selection.size()-1) ? "" : " "); 
    }
    return res;
}

void SentenceFormingState::CreateSparks()
{
    for(int i = 0; i < mNumSparks; i++)
    {
        CreateRandomSpark();
    }
}

void SentenceFormingState::PreBakeLines()
{
    mScene->Start();
    for(int i = 0; i < 50; i++)
    {
        mScene->Update(1 / 60.0f);
        UpdateCursorLine(1 / 60.0f);
    }
}

void SentenceFormingState::CreateRandomSpark()
{
    glm::vec2 pos = glm::vec2(rand() % mGame->GetWidth() , rand() % mGame->GetHeight());
    glm::vec4 startColor = glm::vec4(1,1,1,0.25f);
    glm::vec4 endColor = glm::vec4(1,1,1,0);
    int length = rand()%15 + 3;
    float res = 0.01f;
    int width = 2;
    float xVel = mLineSpeed * ((rand() % 100) / 100.0f * 0.5f + 0.9f);

    Opal::Entity *mSparkEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent();
    transform->Position = glm::vec3(pos.x, pos.y, 0);
    mSparkEntity->AddComponent(transform);
    Opal::VelocityComponent *velocity = new Opal::VelocityComponent();
    velocity->SetVelocity(glm::vec3(xVel,0,0));
    mSparkEntity->AddComponent(velocity);
    AttractableComponent *attr = new AttractableComponent(true);
    mSparkEntity->AddComponent(attr);
    SparkComponent *spark = new SparkComponent(width, startColor, endColor, length, res);
    mSparkEntity->AddComponent(spark);
    DragComponent *drag = new DragComponent(1);
    mSparkEntity->AddComponent(drag);
    NoiseMoveComponent *mover = new NoiseMoveComponent(1,1, 0.05f, rand() % 10000);
    mSparkEntity->AddComponent(mover);

    mScene->AddEntity(mSparkEntity);
    mSparkEntities.push_back(mSparkEntity);
}

void SentenceFormingState::RenderSparks()
{
    std::vector<int> deleteIds;

    for(int i = 0; i < mSparkEntities.size(); i++)
    {
        SparkComponent *spark = mSparkEntities[i]->GetComponent<SparkComponent>();
        Opal::TransformComponent *trans = mSparkEntities[i]->GetComponent<Opal::TransformComponent>();

        if(trans->Position.x > mGame->GetWidth())
        {
            mScene->RemoveEntity(mSparkEntities[i]);
            deleteIds.push_back(i);
            CreateRandomSpark();
        }

        SparkTrail data = spark->TrailData;

        for(int j = data.Points.size()-1; j > 1; j--)
        {
            glm::vec4 currentColor = data.StartColor;
            currentColor.r = Opal::OpalMath::Lerp(currentColor.r, data.EndColor.r, (float)(data.Points.size() - j) / (float) data.Points.size());
            currentColor.g = Opal::OpalMath::Lerp(currentColor.g, data.EndColor.g, (float)(data.Points.size() - j) / (float) data.Points.size());
            currentColor.b = Opal::OpalMath::Lerp(currentColor.b, data.EndColor.b, (float)(data.Points.size() - j) / (float) data.Points.size());
            currentColor.a = Opal::OpalMath::Lerp(currentColor.a, data.EndColor.a, (float)(data.Points.size() - j) / (float) data.Points.size());

            mLineRenderer->DrawLine(data.Points[j-1],data.Points[j], currentColor, data.Width);
        }
    }

    std::reverse(deleteIds.begin(), deleteIds.end());

    for(int i = 0; i < deleteIds.size(); i++)
    {
        mSparkEntities.erase(mSparkEntities.begin() + deleteIds[i]);
    }
}