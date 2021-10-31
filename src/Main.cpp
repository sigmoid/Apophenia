#include <iostream>

#include "../Opal/Game.h"

#include "Gamestate/PlayState.h"
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

    Opal::Game game;
    game.Init(1920, 1080, "Tightrope", Opal::RendererType::VULKAN);

    game.SetFramerateLock(60);
    game.ToggleDebugInfo(true);

    game.Renderer->CreateOrthoCamera(1920, 1080, -1000, 1000);

    DialogueManager dialogue("../Dialogue/TestDialogue.xml");
    Opal::AudioClip mClip("../Audio/test.wav");

    game.PushState<PlayState>();
//    game.PushState<NarrativeState>();
    game.Resize(1920/2, 1080/2);
    // Hacky way to initialize the renderers for the sentence forming state before the window can be resized
    game.PushState<SentenceFormingState>();
    game.PopState();
    game.PushState<EndState>();
    game.PopState();
    game.PushState<PillState>();
    game.PopState();
    game.PushState<StrikesState>();
    game.PopState();

    std::vector<Opal::AudioClipInstance *> mInstances;
    
    bool lastG = false;

    while(!game.ShouldEnd())
    {
        if(Opal::InputHandler::GetKey(GLFW_KEY_G) && !lastG)
        {
            float pan = 0;
            if(Opal::InputHandler::GetKey(GLFW_KEY_LEFT))
            {
                pan = -1;
            }
            if(Opal::InputHandler::GetKey(GLFW_KEY_RIGHT))
            {
                pan = 1;
            }
            mInstances.push_back(game.mAudioEngine.PlaySound(&mClip, 0.5f, 1.0f, pan, false, true));
        }

        for(int i = 0; i < mInstances.size(); i++)
        {
            if(Opal::InputHandler::GetKey(GLFW_KEY_UP))
            {
                mInstances[i]->SetVolume(0.1f);
            }
            else
            {
                mInstances[i]->SetVolume(0.5f);
            }
        }

        lastG = Opal::InputHandler::GetKey(GLFW_KEY_G);
        game.Tick();
    }
}