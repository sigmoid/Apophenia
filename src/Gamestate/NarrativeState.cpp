#include "NarrativeState.h"

NarrativeState::NarrativeState()
{

}

NarrativeState::~NarrativeState()
{
    
}

void NarrativeState::Tick()
{
    mTimeSinceBegin += mGame->GetDeltaTime();
}

void NarrativeState::Render()
{
    GenerateNoise();
    mFontRenderer->RenderString("Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! \nHello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world!\nHello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world!\nHello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world!\nHello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world! Hello, world!", 0, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true);

    mBGPass->Record();
    mBGPass->EndRecord();

    mPass->Record();
    mFontRenderer->RecordCommands();
    mPass->EndRecord();

    mGame->Renderer->SubmitRenderPass(mBGPass);
    mGame->Renderer->SubmitRenderPass(mPass);

    mPostFX->ProcessAndSubmit();
}

void NarrativeState::GenerateNoise()
{
    FastNoiseLite generator;
    generator.SetFrequency(mFrequency);

    for(int y = 0; y < 1080; y++)
    {
        for(int x = 0; x < 1920; x++)
        {
            mNoiseValues.noise[y * 1920 + x] = generator.GetNoise((float)x/(1920.0f * mNoiseScale), (float)y/(1080.0f * mNoiseScale), mTimeSinceBegin);
        }
    }

    void *data = malloc(sizeof(mNoiseValues));
    memcpy(data, &mNoiseValues, sizeof(mNoiseValues));
    mPostFX->UpdateUserData(data);
}

void NarrativeState::Begin()
{
    mTimeSinceBegin = 0;

    mBGPass = mGame->Renderer->CreateRenderPass(true);
    mBGPass->SetClearColor(0,1,0,1);
    mOutputTex = mGame->Renderer->CreateRenderTexture(mGame->GetWidth(),mGame->GetHeight(), 4);
    mPass = mGame->Renderer->CreateRenderPass(mOutputTex, true);
    mPass->SetClearColor(1,0,0,0);

    mFont = std::make_shared<Opal::Font>(mGame->Renderer,Opal::GetBaseContentPath().append("Fonts/JosefinSans.ttf").c_str(), 64);
    mFontRenderer = mGame->Renderer->CreateFontRenderer(mPass, *mFont, glm::vec2(mGame->GetWidth(), mGame->GetHeight()), Opal::Camera::ActiveCamera);

    mPostFX = mGame->Renderer->CreatePostProcessor(mPass, Opal::GetBaseContentPath().append("shaders/testFXVert"), Opal::GetBaseContentPath().append("shaders/testFXFrag"), false, sizeof(NoiseData), VK_SHADER_STAGE_FRAGMENT_BIT);
    GenerateNoise();
}

void NarrativeState::End()
{

}

void NarrativeState::Resume()
{

}

