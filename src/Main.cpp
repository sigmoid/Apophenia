#include <iostream>

#include "../Opal/Game.h"

#include "Gamestate/PromptState.h"
#include "Gamestate/ManagerState.h"
#include "Gamestate/SentenceFormingState.h"
#include "Gamestate/PillState.h"
#include "Gamestate/NarrativeState.h"
#include "Gamestate/EndState.h"
#include "DialogueSystem/DialogueManager.h"
#include "Gamestate/StrikesState.h"

#include "../Opal/Audio/AudioClip.h"
#include "../Opal/Audio/AudioClipInstance.h"
#include "../Opal/Audio/AudioEngine.h"


int main(int argc, char **argv)
{

    std::shared_ptr<Opal::Game> game = std::make_shared<Opal::Game>();
    game->Init(1920, 1080, "Tightrope", Opal::RendererType::VULKAN);

    game->SetFramerateLock(60);
    game->ToggleDebugInfo(true);

    auto cam = game->Renderer->CreateOrthoCamera(1920, 1080, -1000, 1000);
    game->Resize(1920/2, 1080/2);

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
    
    managerState->StartGame(); 

    while(!game->ShouldEnd())
    {
        game->Tick();
    }
}
