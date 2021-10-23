#include "SentenceFormingState.h"

#include "../../Opal/EntityComponent/Entity.h"
#include "../../Opal/EntityComponent/Component.h"
#include "../../Opal/EntityComponent/TransformComponent.h"
#include "../../Opal/EntityComponent/SpriteComponent.h"
#include "../../Opal/EntityComponent/BoxColliderComponent2D.h"
#include "../../Opal/EntityComponent/VelocityComponent.h"
#include "../../Opal/Collision/AABBCollision.h"
#include "../../Opal/vendor/FastNoiseLite.h"
#include "../../Opal/vendor/imgui/implot.h"
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

#include "../../Opal/Logger.h"

#include "EndState.h"

// Static members
Opal::FontRenderer *SentenceFormingState::mTextRenderer = nullptr;
Opal::FontRenderer *SentenceFormingState::mResponseRenderer = nullptr;
Opal::RenderPass *SentenceFormingState::mTextPass = nullptr;
Opal::RenderPass *SentenceFormingState::mBGPass = nullptr;
Opal::BatchRenderer2D *SentenceFormingState::mBatch = nullptr;
Opal::MeshRenderer2D *SentenceFormingState::mMeshRenderer = nullptr;
Opal::Texture *SentenceFormingState::mCursorTexture = nullptr;
Opal::LineRenderer *SentenceFormingState::mLineRenderer = nullptr;
Opal::Font *SentenceFormingState::mFont = nullptr;
Opal::Font *SentenceFormingState::mResponseFont = nullptr;
Opal::PostProcessRenderer *SentenceFormingState::mPostProcess = nullptr;
Opal::Texture *SentenceFormingState::mRenderTexture = nullptr;

SentenceFormingState::SentenceFormingState()
{
}

void SentenceFormingState::Tick()
{
    mTimeSinceBirth += mGame->GetDeltaTime();

    // if(Opal::InputHandler::GetKey(GLFW_KEY_ESCAPE))
    // {
    //     mGame->PopState();
    //     return;
    // }

    if(Opal::InputHandler::GetKey(GLFW_KEY_K))
    {
        mKillEventTimer = mKillWaitTime;
    }

    if(mKillEventTimer > 0)
    {
        mKillEventTimer -= mGame->GetDeltaTime();
        mWarpFactor = Opal::OpalMath::Lerp(mBaseWarp, mMaxWarp, 1.0f - mKillEventTimer/mKillWaitTime);

        mSparkSpeedUp = Opal::OpalMath::Lerp(1.0f, mMaxSparkSpeedUp, 1.0f - mKillEventTimer/mKillWaitTime);
        

        if(mKillEventTimer <= 0)
        {
            mKillEventTimer = 0;
            mIsWarped = true;
            mZoomTimer = mZoomDuration;
            //mCursorEntity->GetComponent<CursorComponent>()->Kill();
        }
    }
    else if(mZoomTimer > 0)
    {
        mScreenShakeTimer = 0;
        mZoomTimer -= mGame->GetDeltaTime();
        if(mZoomTimer <= 0)
        {
            mZoomTimer = 0;
            mZoom2Timer = mZoom2Duration;
        }

        float progress = 1.0f - mZoomTimer/mZoomDuration;
        mCurrentZoom = Opal::OpalMath::Lerp(1.0f, mZoomTarget, progress);
        Opal::Camera::ActiveCamera->SetZoom(mCurrentZoom);
        float translation = 1080.0f / mCurrentZoom;
        translation = translation - 1080.0f;
        translation /= 2.0f;

        Opal::Camera::ActiveCamera->MoveCamera(glm::vec2(0, translation));
    }
    else if(mZoom2Timer > 0)
    {
        mScreenShakeTimer = 0;
        mZoom2Timer -= mGame->GetDeltaTime();
        if(mZoom2Timer <= 0)
        {
            mZoom2Timer = 0;

            Opal::Camera::ActiveCamera->SetZoom(1);
            Opal::Camera::ActiveCamera->MoveCamera(glm::vec2(0,0));
            DialogueManager::Instance->ProcessResponse("KILL");
            DialogueManager::Instance->EatTransition = true;
            mGame->PopState();
            return;
        }

        float progress = 1.0f - mZoom2Timer/mZoom2Duration;
        progress = pow(progress, mZoom2Pow);
        mCurrentZoom = Opal::OpalMath::Lerp(mZoomTarget, mZoom2Target, progress);
        Opal::Camera::ActiveCamera->SetZoom(mCurrentZoom);
        float translation = 1080.0f / mCurrentZoom;
        translation = translation - 1080.0f;
        translation /= 2.0f;

        mWarpFactor = Opal::OpalMath::Lerp(mMaxWarp, mBaseWarp, progress);

        Opal::Camera::ActiveCamera->MoveCamera(glm::vec2(0, translation));
    }
    else if (mScreenShakeTimer > 0)
    {
        Opal::Camera::ActiveCamera->MoveCamera(glm::vec2((rand() % mScreenShakeIntensity * 1000) / 1000.0f - (float)mScreenShakeIntensity / 2, (rand() % mScreenShakeIntensity * 1000) / 1000.0f - (float)mScreenShakeIntensity / 2));
        mScreenShakeTimer -= mGame->GetDeltaTime();

        if (mScreenShakeTimer <= 0)
        {
            Opal::Camera::ActiveCamera->MoveCamera(glm::vec2(0, 0));
        }
    }
    mScene->Update(mGame->GetDeltaTime());
    UpdateCursorLine();
    if (!mCursorEntity->GetComponent<CursorComponent>()->GetAlive())
    {
        auto response = mCursorEntity->GetComponent<CursorComponent>()->GetResponse();
        std::string responseStr = ConcatSelection(response);

        if(mCursorEntity->GetComponent<CursorComponent>()->GetKill())
        {
            DialogueManager::Instance->ProcessResponse("KILL");
            free(mScene);
            mGame->PopState();
            return;
        }
        else if (!DialogueManager::Instance->ProcessResponse(responseStr))
        {
            for (int i = 0; i < mFragmentEnts.size(); i++)
            {
                mScene->RemoveEntity(mFragmentEnts[i]);
            }
            mFragmentEnts.clear();
            mScene->RemoveEntity(mEndWall);

            mCursorEntity->GetComponent<CursorComponent>()->Reset();

            if(!mIsWarped)
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

void SentenceFormingState::RenderBlackHole()
{
    std::vector<float> verts;

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(mBHNoiseFreq);
    
    glm::vec2 startPos = mBHPos;
    glm::vec2 lastPos = startPos;
    glm::vec2 firstPos = lastPos;
    float firstRadius = 0;
    float lastRadius = 0;

    float noiseSeed = mTimeSinceBirth;
    bool first = true;

    for(int i = 0; i < mNumTris; i++)
    {
        float noiseVal = noise.GetNoise<float>((i * mNoiseScale),noiseSeed * mBHNoiseTurbulence) * mBHNoiseIntensity;
        glm::vec2 curPos = mBHPos;

        float theta = ((float)i / (float)mNumTris) * 3.14159f + 3.14159f/2.0f;

        float newRadius = mBHRadius + noiseVal;

        curPos.x += cos(theta) * newRadius;
        curPos.y += sin(theta) * newRadius;

        verts.push_back(startPos.x);
        verts.push_back(startPos.y);
        verts.push_back(mBHColor.r);
        verts.push_back(mBHColor.g);
        verts.push_back(mBHColor.b);
        verts.push_back(mBHColor.a);
        verts.push_back(lastPos.x);
        verts.push_back(lastPos.y);
        verts.push_back(mBHColor.r + 0.03f);
        verts.push_back(mBHColor.g + 0.03f);
        verts.push_back(mBHColor.b + 0.03f);
        verts.push_back(mBHColor.a);
        verts.push_back(curPos.x);
        verts.push_back(curPos.y);
        verts.push_back(mBHColor.r+ 0.03f);
        verts.push_back(mBHColor.g+ 0.03f);
        verts.push_back(mBHColor.b+ 0.03f);
        verts.push_back(mBHColor.a);

        if(first)
        {
            firstPos = curPos;
            first = false;
        }
        else if(i == mNumTris - 1) //last
        {
            verts.push_back(startPos.x);
            verts.push_back(startPos.y);
            verts.push_back(mBHColor.r);
            verts.push_back(mBHColor.g);
            verts.push_back(mBHColor.b);
            verts.push_back(mBHColor.a);
            verts.push_back(curPos.x);
            verts.push_back(curPos.y);
            verts.push_back(mBHColor.r);
            verts.push_back(mBHColor.g);
            verts.push_back(mBHColor.b);
            verts.push_back(mBHColor.a);
            verts.push_back(firstPos.x);
            verts.push_back(firstPos.y);
            verts.push_back(mBHColor.r);
            verts.push_back(mBHColor.g);
            verts.push_back(mBHColor.b);
            verts.push_back(mBHColor.a);
        }
        lastPos = curPos;
    }
    mBHMesh->SetVertices(verts.data(), verts.size() * sizeof(float));

    mMeshRenderer->Submit(mBHMesh);
}

void SentenceFormingState::Render()
{
    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplGlfw_NewFrame();

    // ImGui::NewFrame();

    // ImGui::Begin("Noise settings");

    // ImGui::DragFloat("CurrentZoom", &mCurrentZoom, 0.05f);

    // ImGui::DragFloat("Zoom 1 Duration", &mZoomDuration);
    // ImGui::DragFloat("Zoom 1 Target", &mZoomTarget);

    // ImGui::DragFloat("Zoom 2 Duration", &mZoom2Duration);
    // ImGui::DragFloat("Zoom 2 Target", &mZoom2Target);
    // ImGui::DragFloat("Zoom 2 Pow", &mZoom2Pow);

    // ImGui::DragFloat("Kill Wait", &mKillWaitTime);

    // ImGui::End();

    // ImGui::Render();
    // ImGui::EndFrame();

    mBatch->StartBatch();
    mScene->Render(mBatch);
    mBatch->RenderBatch();

    mBGPass->Record();
    mBGPass->EndRecord();
    mGame->Renderer->SubmitRenderPass(mBGPass);

    // dEBUG mLineRenderer->DrawLine(glm::vec2(0,0), glm::vec2(1000,1000), glm::vec4(1,0,0,1), 3);
    DrawCursorLine();
    RenderSparks();


    mMeshRenderer->StartFrame();
    mMeshRenderer->Submit(mCursorEntity->GetComponent<CursorComponent>()->GetMesh());

    //mTextRenderer->RenderString("This is a response!", 1920/2 - 300, 1080/2, 0.9f, 0.9f, 0.9f, 1.0f, 1.0f);
    RenderSentenceFragments();
    if(!mIsWarped)
        RenderCurrentSelection();

    RenderBlackHole();

    mTextPass->Record();
    mBatch->RecordCommands();
    mLineRenderer->Render();
    mTextRenderer->RecordCommands();
    mResponseRenderer->RecordCommands();
    mMeshRenderer->RecordCommands();
    mTextPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mTextPass);

    UBO * ubo = new UBO();
    ubo->warpFactor = mWarpFactor;
    mPostProcess->UpdateUserData(ubo);
    mPostProcess->ProcessAndSubmit();
}

void SentenceFormingState::Begin()
{
    Opal::Logger::LogString("GAMESTATE: Begin() SentenceFormingState");
    if (mTextRenderer == nullptr)
    {
        mRenderTexture = mGame->Renderer->CreateRenderTexture(1920, 1080,4);
        mTextPass = mGame->Renderer->CreateRenderPass(mRenderTexture, true);
        mTextPass->SetClearColor(0.1f,0.1f,0.1f,1.0f);
        mPostProcess = mGame->Renderer->CreatePostProcessor(mTextPass, "../shaders/testFXvert", "../shaders/testFXfrag", false, sizeof(UBO), VK_SHADER_STAGE_FRAGMENT_BIT);
        mBGPass = mGame->Renderer->CreateRenderPass(true);
        mBGPass->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        mFont = new Opal::Font(mGame->Renderer, "../fonts/JosefinSans-Light.ttf", 90);
        mResponseFont = new Opal::Font(mGame->Renderer, "../fonts/JosefinSans-Light.ttf", 64);

        mMeshRenderer = mGame->Renderer->CreateMeshRenderer(mTextPass);

        mTextRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mFont, glm::vec2(1920 - 200, mGame->GetHeight()), Opal::Camera::ActiveCamera);
        mLineRenderer = new Opal::LineRenderer();
        mLineRenderer->Init(mGame->Renderer, mTextPass, true);

        std::vector<Opal::Texture *> textures;
        mCursorTexture = mGame->Renderer->CreateTexture("../textures/cursor.png");
        textures.push_back(mCursorTexture);
        mBatch = mGame->Renderer->CreateBatch(mTextPass, 1000, textures, true);

        mResponseRenderer = mGame->Renderer->CreateFontRenderer(mTextPass, *mResponseFont, glm::vec2(1920 - 200, mGame->GetHeight()), Opal::Camera::ActiveCamera);
    }
    mScene = new Opal::Scene(mBatch);

    mNoise.SetSeed(rand());
    mNoise.SetFrequency(mNoiseFrequency);

    std::string wbPath = "../Dialogue/DefaultWordBank.xml";
    Response response = DialogueManager::Instance->GetCurrentResponse();
    if(response.WordFrequency != 0 && response.WordBank != "")
    {
        wbPath = response.WordBank;
    }
    mWordBank = DialogueSerializer::GetWordBank(wbPath);

    mChannelHeight = response.ChannelSize;

    CreatePlayer();
    CreateSparks();
    PreBakeLines();

    CreateBounds();
    CreatePlayingField();

    mBHMesh = mGame->Renderer->CreateMesh((mNumTris + 1) * 3);

    if(DialogueManager::Instance->GetCurrentPrompt().IsKill)
    {
        mKillEventTimer = mKillWaitTime;
    }
}

void SentenceFormingState::CreateBounds()
{
    Opal::Entity *mTopBounds = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent(glm::vec3(0, -100, 0), glm::vec3(1, 1, 1), 0);
    mTopBounds->AddComponent(transform);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(600, 100), glm::vec2(0, 0), true);
    collider->SetIsTrigger(false);
    collider->SetIsStatic(true);
    mTopBounds->AddComponent(collider);

    mScene->AddEntity(mTopBounds);

    Opal::Entity *mBottomBounds = new Opal::Entity();

    Opal::TransformComponent *transform2 = new Opal::TransformComponent(glm::vec3(0, 1080, 0), glm::vec3(1, 1, 1), 0);
    mBottomBounds->AddComponent(transform2);
    Opal::BoxColliderComponent2D *collider2 = new Opal::BoxColliderComponent2D(glm::vec2(600, 100), glm::vec2(0, 0), true);
    collider->SetIsTrigger(false);
    collider->SetIsStatic(true);
    mBottomBounds->AddComponent(collider2);

    mScene->AddEntity(mBottomBounds);
}

void SentenceFormingState::CreatePlayingField()
{
    Response resp = DialogueManager::Instance->GetCurrentResponse();

    float start = 1920;
    float inc = mSpaceBetweenFragments;

    mLineSpeed = resp.Speed;
    for (int i = 0; i < resp.Fragments.size(); i++)
    {
        float ypos;
        float xpos = start + inc * i;
        ypos = mNoise.GetNoise((float)(xpos +mCurrentScroll)/ mNoiseScale, 0.0f);
        ypos += 1.0f;
        ypos /= 4.0f;
        // ypos now represents the top of the channel
        ypos = ypos * (1080 - mPadding * 2) + mPadding;
        float subFrag = resp.Fragments[i].size();
        
        for (int j = 0; j < subFrag; j++)
        {
            float finalY = ypos;
            float yStep = mChannelHeight / (subFrag + 1);
            finalY += yStep * (j+1);
            CreateSentenceFragment(glm::vec3(xpos, finalY, 0), resp.Fragments[i][j].Text, resp.Fragments[i][j].Attraction, resp.Speed, resp.Fragments[i][j].IsIntrusive, true, true);
        }
    }

    // Create "background" words
    for(int i = 0; i < resp.WordFrequency * 10; i++)
    {
        float xpos = rand() % (int)(start + inc * (resp.Fragments.size()) - 1500) + 1500;
        float channelTop = mNoise.GetNoise((float)(xpos +mCurrentScroll)/ mNoiseScale, 0.0f);
        channelTop += 1.0f;
        channelTop /= 4.0f;
        // channelTop now represents the top of the channel
        channelTop = channelTop * (1080 - mPadding * 2) + mPadding;

        float ypos = rand() % 1080;

        while(ypos > channelTop && ypos < channelTop + mChannelHeight)
        {
            ypos = rand() % 1080;
        }
        
        CreateSentenceFragment(glm::vec3(xpos, ypos, 0), mWordBank[rand()%mWordBank.size()].Text, 0, resp.Speed, resp.SolidWords, resp.SolidWords, false);
    }

    CreateEndWall(start + inc * (resp.Fragments.size()));
    mCurrentScroll += start + inc * (resp.Fragments.size());
}

void SentenceFormingState::End()
{
    Opal::Logger::LogString("GAMESTATE: End() SentenceFormingState");
}

void SentenceFormingState::Resume()
{
}

void SentenceFormingState::CreateEndWall(float x)
{
    Opal::Entity *mEndWallEnt = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent(glm::vec3(x, 0, 0), glm::vec3(1, 1, 1), 0);
    mEndWallEnt->AddComponent(transform);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(600, 2000), glm::vec2(0, 0), true);
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

    Opal::TransformComponent *transform = new Opal::TransformComponent(glm::vec3(1920 / 4, 1080 / 2, 0), glm::vec3(1, 1, 1), 0);
    mCursorEntity->AddComponent(transform);
    //Opal::SpriteComponent *sprite = new Opal::SpriteComponent(mCursorTexture);
    //mCursorEntity->AddComponent(sprite);
    CursorComponent *cursor = new CursorComponent();
    mCursorEntity->AddComponent(cursor);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(64, 64), glm::vec2(-32, -32), false);
    mCursorEntity->AddComponent(collider);
    Opal::VelocityComponent *velocity = new Opal::VelocityComponent();
    mCursorEntity->AddComponent(velocity);
    AttractableComponent *attr = new AttractableComponent(true);
    mCursorEntity->AddComponent(attr);
    DragComponent *drag = new DragComponent(1);
    mCursorEntity->AddComponent(drag);

    mScene->AddEntity(mCursorEntity);
}

void SentenceFormingState::CreateSentenceFragment(glm::vec3 pos, std::string text, float attraction, float speed, bool intrusive, bool solid, bool core)
{
    //Kludge, haven't actually implemented text measuring yet
    float width = mTextRenderer->MeasureText(text);

    Opal::Entity *mFragmentEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent(pos, glm::vec3(1, 1, 1), 0);
    mFragmentEntity->AddComponent(transform);
    SentenceFragmentComponent *frag = new SentenceFragmentComponent(text, speed, mFragmentColor, attraction, intrusive, solid, core);
    mFragmentEntity->AddComponent(frag);
    Opal::BoxColliderComponent2D *collider = new Opal::BoxColliderComponent2D(glm::vec2(fmax(64, width), mFragmentSize), glm::vec2(0, -mFragmentSize), true);
    collider->SetIsTrigger(true);
    collider->SetIsStatic(true);
    mFragmentEntity->AddComponent(collider);

    if(!frag->GetCore())
    {
        for(int i = 0; i < mFragmentEnts.size(); i++)
        {
            Opal::BoxColliderComponent2D * otherCollider = mFragmentEnts[i]->GetComponent<Opal::BoxColliderComponent2D>();   
            SentenceFragmentComponent * otherFrag = mFragmentEnts[i]->GetComponent<SentenceFragmentComponent>();

            if(!otherFrag->GetCore())
                continue; 

            if(Opal::AABBCollision::GetResolution(otherCollider->GetAABB(), collider->GetAABB()) != glm::vec2(0,0))
            {
                free(mFragmentEntity);
                return;
            } 
        }
    }

    mScene->AddEntity(mFragmentEntity);
    mFragmentEnts.push_back(mFragmentEntity);
}

void SentenceFormingState::RenderSentenceFragments()
{
    std::vector<Opal::Entity *> entities = mScene->GetAllEntities();

    for (int i = 0; i < entities.size(); i++)
    {
        SentenceFragmentComponent *frag = entities[i]->GetComponent<SentenceFragmentComponent>();
        if (frag != nullptr)
        {
            glm::vec3 pos = entities[i]->GetComponent<Opal::TransformComponent>()->Position;
            if (frag->Attraction > 0)
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
    if (mLinePoints.size() < 2)
    {
        return;
    }

    for (int i = 1; i < mLinePoints.size(); i++)
    {
        mLineRenderer->DrawLine(mLinePoints[i - 1], mLinePoints[i], mLineColor, 3);
    }

    mLineRenderer->DrawLine(mLinePoints[mLinePoints.size()-1], glm::vec2(mCursorEntity->GetComponent<Opal::TransformComponent>()->Position.x, mCursorEntity->GetComponent<Opal::TransformComponent>()->Position.y), 
    glm::vec4(mLineColor.r, mLineColor.g, mLineColor.b, mLineColor.a * mCurrentZoom), 3);
}

void SentenceFormingState::UpdateCursorLine(float timeOverride)
{
    int cutoffpoint = -1;
    for (int i = 0; i < mLinePoints.size(); i++)
    {
        mLinePoints[i].x -= mLineSpeed * ((timeOverride > 0) ? timeOverride : mGame->GetDeltaTime());
        if (mLinePoints[i].x < mLineCutoff)
            cutoffpoint = i;
    }

    if (cutoffpoint != -1)
    {
        mLinePoints.erase(mLinePoints.begin(), mLinePoints.begin() + cutoffpoint);
    }
    mLineTimer -= mGame->GetDeltaTime();
    if (mLineTimer <= 0)
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
    mResponseRenderer->RenderString(resp, 200, 1080 - 150, mFragmentColor.r, mFragmentColor.g, mFragmentColor.b, mFragmentColor.a, 1.0f);
}

std::string SentenceFormingState::ConcatSelection(std::vector<std::string> selection)
{
    std::string res = "";
    for (int i = 0; i < selection.size(); i++)
    {
        res += selection[i] + ((i == selection.size() - 1) ? "" : " ");
    }
    return res;
}

void SentenceFormingState::CreateSparks()
{
    Response resp = DialogueManager::Instance->GetCurrentResponse();
    mLineSpeed = resp.Speed;
    for (int i = 0; i < mNumSparks; i++)
    {
        CreateRandomSpark();
    }
}

void SentenceFormingState::PreBakeLines()
{
    mScene->Start();
    for (int i = 0; i < 200; i++)
    {
        mScene->Update(1 / 60.0f);
        UpdateCursorLine(1 / 60.0f);
    }
}

void SentenceFormingState::CreateRandomSpark()
{
    glm::vec2 pos = glm::vec2(rand() % 1920, rand() % 1080);
    glm::vec4 startColor = glm::vec4(1, 1, 1, 0.25f);
    glm::vec4 endColor = glm::vec4(1, 1, 1, 0);
    int length = rand() % 15 + 3;
    float res = 0.01f;
    int width = 2;
    float xVel = mLineSpeed * ((rand() % 100) / 100.0f * 0.5f + 0.9f);

    Opal::Entity *mSparkEntity = new Opal::Entity();

    Opal::TransformComponent *transform = new Opal::TransformComponent();
    transform->Position = glm::vec3(pos.x, pos.y, 0);
    mSparkEntity->AddComponent(transform);
    Opal::VelocityComponent *velocity = new Opal::VelocityComponent();
    velocity->SetVelocity(glm::vec3(xVel, 0, 0));
    mSparkEntity->AddComponent(velocity);
    AttractableComponent *attr = new AttractableComponent(true);
    mSparkEntity->AddComponent(attr);
    SparkComponent *spark = new SparkComponent(width, startColor, endColor, length, res);
    mSparkEntity->AddComponent(spark);
    DragComponent *drag = new DragComponent(1);
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
    NoiseMoveComponent *mover = new NoiseMoveComponent(str, freq, scl, rand() % 10000);
    mSparkEntity->AddComponent(mover);

    mScene->AddEntity(mSparkEntity);
    mSparkEntities.push_back(mSparkEntity);
}

void SentenceFormingState::RenderSparks()
{
    std::vector<int> deleteIds;

    for (int i = 0; i < mSparkEntities.size(); i++)
    {
        SparkComponent *spark = mSparkEntities[i]->GetComponent<SparkComponent>();
        spark->SetSpeedUp((mSparkSpeedUp - 1) * ((float)i/(float)mSparkEntities.size()) + 1);
        Opal::TransformComponent *trans = mSparkEntities[i]->GetComponent<Opal::TransformComponent>();

        if (trans->Position.x > mGame->GetWidth() / mCurrentZoom)
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

// void SentenceFormingState::CreateRandomScrollingWord()
// {
//     glm::vec2 pos = glm::vec2(2000, rand() % 1080);
//     glm::vec4 color = glm::vec4(1, 1, 1, 0.125f);
//     float speed = mLineSpeed;// * ((rand() % 100) + 50) / 100.0f;

//     bool solid = DialogueManager::Instance->GetCurrentResponse().SolidWords;

//     if(solid)
//     {
//         color = glm::vec4(1,1,1,1);
//     }


//     FastNoiseLite noise(23);
//     noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
//     noise.SetFrequency(mNoiseFrequency);

//     pos.y = (noise.GetNoise((mCurrentScroll/mSpaceBetweenFragments) / mNoiseScale, (float)(0)) + 1.0f);
//     pos.y /= 2.0f;    
//     pos.y  *= (1080 - mPadding * 2) + mPadding;

//     bool up = (rand()%50 > 25);

//     //pos.y += (rand()%500 + 100) * (up? 1:-1);

//     Opal::Entity *mScrollEntity = new Opal::Entity();

//     Opal::TransformComponent *transform = new Opal::TransformComponent();
//     transform->Position = glm::vec3(pos.x, pos.y, 0);
//     mScrollEntity->AddComponent(transform);
//     ScrollingWordComponent *word = new ScrollingWordComponent(mWordBank[rand()%mWordBank.size()].Text, speed, color);
//     mScrollEntity->AddComponent(word);

//     for(int i = 0; i < mScrollingWords.size(); i++)
//     {
//         if(rand() % 100 < 20)
//         {
//             mWordConnections.push_back(std::make_pair(mScrollingWords[i], mScrollEntity));
//         }
//     }


//     mScene->AddEntity(mScrollEntity);
//     mScrollingWords.push_back(mScrollEntity);
// }

// void SentenceFormingState::UpdateScrollingWords()
// {
//     std::vector<int> remove;
//     for(int i = 0; i < mScrollingWords.size();i++)
//     {
//         if(mScrollingWords[i]->GetComponent<Opal::TransformComponent>()->Position.x < -500)
//         {
//             std::vector<int> removeJ;
//             for(int j = 0; j < mWordConnections.size(); j++)
//             {
//                 if(mWordConnections[j].first == mScrollingWords[i] || mWordConnections[j].second == mScrollingWords[i])
//                 {
//                     removeJ.push_back(j);
//                 }
//             }

//             for(int j = removeJ.size()-1; j >= 0 ; j--)
//             {
//                 mWordConnections.erase(mWordConnections.begin() + removeJ[j]);
//             }

//             mScene->RemoveEntity(mScrollingWords[i]);
//             remove.push_back(i);
//         }
//     }

//     for(int i = remove.size()-1; i >= 0; i--)
//     {
//         mScrollingWords.erase(mScrollingWords.begin() + remove[i]);
//     }
// }
