#include <iostream>

#include "../Opal/util.h"

#include "Gamestate/PromptState.h"
#include "Gamestate/ManagerState.h"
#include "Gamestate/SentenceFormingState.h"
#include "Gamestate/PillState.h"
#include "Gamestate/NarrativeState.h"
#include "Gamestate/MainMenuState.h"
#include "Gamestate/EndState.h"
#include "DialogueSystem/DialogueManager.h"
#include "Gamestate/StrikesState.h"
#include "Gamestate/CreditsState.h"
#include "Gamestate/LogoState.h"

#include "../Opal/Audio/AudioClip.h"
#include "../Opal/Audio/AudioClipInstance.h"
#include "../Opal/Audio/AudioEngine.h"
#include "GameSettings.h"

#include <cstdio>

#ifdef __IPHONEOS__
#include "Main.h"

int MainClass::CallMain(int argc, char ** argv)
{
    main(argc, argv);
}

int MainClass::main(int argc, char *argv[])
#else
int main(int argc, char * argv[])
#endif
{
    int width = 1920;
    int height = 1080;
    
    std::shared_ptr<Opal::Game> game = std::make_shared<Opal::Game>();
    game->Init(width, height, "Of Moons and Mania", Opal::RendererType::VULKAN);

    game->SetFramerateLock(60);
    //game->ToggleDebugInfo(false);
    
    auto cam = game->Renderer->CreateOrthoCamera(game->GetWidth(), game->GetHeight(), -1000, 1000);
    cam->Resize(game->GetWidth(), game->GetHeight());
    

    DialogueManager dialogue("../Dialogue/TestDialogue.xml");

    GameSettings::Load();


    auto managerState = game->PushState<ManagerState>();
//    game->PushState<NarrativeState>();
    game->PushState<PromptState>();
    game->PopState();
    // Hacky way to initialize the renderers for the sentence forming state before the window can be resized
    game->PushState<SentenceFormingState>();
    game->PopState();
    game->PushState<EndState>();
    game->PopState();
    game->PushState<PillState>();
    game->PopState();
    game->PushState<StrikesState>();
    game->PopState();
    game->PushState<CreditsState>();
    game->PopState();
    game->PushState<MainMenuState>();
    game->PushState<LogoState>();
    
    
    auto bgMusic = Opal::AudioEngine::LoadClip(Opal::GetBaseContentPath().append("Audio/Canone_W.mp3"));
    Opal::AudioEngine::PlaySound(bgMusic, 0.8f, 1.0f, 0.0f, true, false);

    bool lastF11 = false;
    bool lastEsc = false;
    
    float resizeTimer = 0.1;
    bool hasResized = false;

    managerState->StartGame(); 

    while(!game->ShouldEnd())
    {
        if(!hasResized)
        {
            resizeTimer -= game->GetDeltaTime();
            if(resizeTimer <= 0)
            {
                game->Resize(1920/2, 1080/2);
                hasResized = true;
            }
        }
        //if (Opal::InputHandler::GetKey(GLFW_KEY_F11) && !lastF11)
        //{
        //    game->ToggleFullscreen();
        //}
        if ((Opal::InputHandler::GetKey(SDL_SCANCODE_ESCAPE) || Opal::InputHandler::GetGamepadButton(SDL_CONTROLLER_BUTTON_START)) && std::dynamic_pointer_cast<MainMenuState>(game->PeekState()) == nullptr)
        {
            if (std::dynamic_pointer_cast<CreditsState>(game->PeekState()) != nullptr || std::dynamic_pointer_cast<LogoState>(game->PeekState()) != nullptr)
                game->PopState();
            else
            {
                if (game->PeekState() == managerState)
                    managerState->Pause();
                game->PushState<MainMenuState>();
            }
        }
        game->Tick();
    }

    return 0;
}

//test
