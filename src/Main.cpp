#include <iostream>

#include "../Opal/Game.h"

#include "Gamestate/PlayState.h"
#include "Gamestate/SentenceFormingState.h"
#include "Gamestate/EndState.h"
#include "DialogueSystem/DialogueManager.h"

int main(int argc, char **argv)
{
    Opal::Game game;
    game.Init(1920, 1080, "Apophenia", Opal::RendererType::VULKAN);

    game.SetFramerateLock(60);
    game.ToggleDebugInfo(false);

    game.Renderer->CreateOrthoCamera(1920, 1080, -1000, 1000);

    DialogueManager dialogue("../Dialogue/TestDialogue.xml");

    game.PushState<PlayState>();

    // Hacky way to initialize the renderers for the sentence forming state before the window can be resized
    game.PushState<SentenceFormingState>();
    game.PopState();
    game.PushState<EndState>();
    game.PopState();

    while(!game.ShouldEnd())
    {
        game.Tick();
    }
}