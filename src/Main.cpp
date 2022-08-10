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

#include "../Opal/Audio/AudioClip.h"
#include "../Opal/Audio/AudioClipInstance.h"
#include "../Opal/Audio/AudioEngine.h"

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
    
#ifndef __IPHONEOS__
    //game->Resize(game->GetWidth()/2, game->GetHeight()/2);
#endif
    
    DialogueManager dialogue("../Dialogue/TestDialogue.xml");


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
    game->PushState<MainMenuState>();
    
    auto bgMusic = Opal::AudioEngine::LoadClip(Opal::GetBaseContentPath().append("Audio/ambiment-by-kevin-macleod-from-filmmusic-io.mp3"));
    Opal::AudioEngine::PlaySound(bgMusic, 0.8f, 1.0f, 0.0f, true, false);

    bool lastF11 = false;
    bool lastEsc = false;

    managerState->StartGame(); 

    while(!game->ShouldEnd())
    {
        //if (Opal::InputHandler::GetKey(GLFW_KEY_F11) && !lastF11)
        //{
        //    game->ToggleFullscreen();
        //}
        if (Opal::InputHandler::GetKey(SDL_SCANCODE_ESCAPE) && std::dynamic_pointer_cast<MainMenuState>(game->PeekState()) == nullptr)
        {
            if(game->PeekState() == managerState)
                managerState->Pause();
            game->PushState<MainMenuState>();
        }
        game->Tick();
    }
}

//test
