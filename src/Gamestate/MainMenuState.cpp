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

#include "EndState.h"
#include "../../Opal/vendor/imgui/implot.h"
#include "../../Opal/vendor/imgui/imgui.h"
#include "../../Opal/vendor/imgui/imgui_impl_vulkan.h"
#include "../../Opal/vendor/imgui/imgui_impl_sdl.h"
#include "../../Opal/Graphics/Vulkan/VulkanRenderer.h"

//std::shared_ptr<Opal::RenderPass> MainMenuState::mRenderPass = nullptr;

MainMenuState::MainMenuState()
{
    
}
MainMenuState::~MainMenuState()
{
    
}

void MainMenuState::Tick() 
{
    mScene->Update(mGame->GetDeltaTime());
    if(Opal::InputHandler::GetTouch())
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::EndFrame();
        mGame->PopState();
        return;
    }
}

void MainMenuState::Render() 
{
    RenderSparks();

    mBatch->StartBatch();
    mScene->Render(mBatch);
    mBatch->RenderBatch();

    mRenderPass->Record();
    mBatch->RecordCommands();
    mLineRenderer->Render();
    mRenderPass->EndRecord();
    mGame->Renderer->SubmitRenderPass(mRenderPass);

    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplSDL2_NewFrame();

    // ImGuiStyle * style = &ImGui::GetStyle();

    // style->WindowPadding = ImVec2(15 * mGame->GetScale().x, 15 * mGame->GetScale().x) ;
	// style->WindowRounding = 5.0f;
	// style->FramePadding = ImVec2(5, 5);
	// style->FrameRounding = 4.0f;
	// style->ItemSpacing = ImVec2(12 * mGame->GetScale().x, mGame->GetHeight()/32 * mGame->GetScale().x);
	// style->ItemInnerSpacing = ImVec2(8 * mGame->GetScale().x, 6 * mGame->GetScale().x);
	// style->IndentSpacing = 25.0f * mGame->GetScale().x;
	// style->ScrollbarSize = 15.0f * mGame->GetScale().x;
	// style->ScrollbarRounding = 9.0f * mGame->GetScale().x;
	// style->GrabMinSize = 5.0f;
	// style->GrabRounding = 3.0f;
    // style->Colors[ImGuiCol_Button]                = ImVec4(0.4375f, 0.4258f, 0.3828f, 0.29f);
    // style->Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.4375f, 0.4258f, 0.3828f, 0.49f);
    // style->Colors[ImGuiCol_ButtonActive]          = ImVec4(0.4375f, 0.4258f, 0.3828f, 0.69f);

    // ImGui::NewFrame();
    // const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    // ImGui::SetNextWindowPos(ImVec2(mGame->GetWidth()/10, mGame->GetHeight() / 10));
    // ImGui::SetNextWindowSize(ImVec2(mGame->GetWidth(), mGame->GetHeight()));
    // ImGui::Begin("Main Menu", nullptr , ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
    
    // ImGui::PushFont(Opal::VulkanRenderer::TitleFont);
    // ImGui::Text("Tightrope");

    // if(mCurrentState == MenuState::Default)
    // {
    //     if(ImGui::Button("Play"))
    //     {
    //         ImGui::PopFont();
    //         ImGui::End();
    //         ImGui::EndFrame();
    //         mGame->PopState();
    //         return;
    //     }

    //     if(ImGui::Button("Credits"))
    //     {
    //         mCurrentState = MenuState::Credits;
    //     }

    //     if(ImGui::Button("Options"))
    //     {
    //         mCurrentState = MenuState::Options;
    //     }

    //     if(ImGui::Button("Exit"))
    //     {
    //         mGame->End();
    //     }
    // }
    // else if(mCurrentState == MenuState::Options)
    // {
    //     float volume = mGame->GetGlobalVolume();
    //     ImGui::DragFloat("Volume", &volume,0.01,0,1);
    //     mGame->SetGlobalVolume(volume);

    //     if(ImGui::Button("Back"))
    //     {
    //         mCurrentState = MenuState::Default;
    //     }
    // }
    // else if (mCurrentState == MenuState::Credits)
    // {
    //     ImGui::PushFont(Opal::VulkanRenderer::NormalFont);
    //     ImGui::Text("Game Made By: Adam Waggoner (@absurd_walls)");
    //     ImGui::Text("Tools Used: MoltenVK, SoLoud Audio, GLFW, ImGui, GLM, TinyXML2");
    //     ImGui::Text("Special Thanks: Mom, Dad, Emily, Michael, Light, Patrick, Skyler,");
    //     ImGui::Text("Jaden, Austin, and Logan for their feedback and support");
    //     ImGui::Text("throughout the development of this game.");
        
    //     ImGui::PopFont();

    //     if (ImGui::Button("Back"))
    //     {
    //         mCurrentState = MenuState::Default;
    //     }
    // }
    // ImGui::PopFont();
    // ImGui::End();

    // ImGui::Render();
    // ImGui::EndFrame();
}

void MainMenuState::Begin() 
{
    mRenderPass = mGame->Renderer->CreateRenderPass(true);
    mRenderPass->SetClearColor(0.2f,0.2f,0.2f,1.0f);

    std::vector<std::shared_ptr<Opal::Texture> > textures;
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
    CreateSparks();
}

void MainMenuState::End() 
{

}

void MainMenuState::Resume() 
{

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
