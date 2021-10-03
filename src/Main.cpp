#include <iostream>

#include "../Opal/Game.h"

#include "Gamestate/PlayState.h"
#include "DialogueSystem/DialogueManager.h"

int main(int argc, char **argv)
{
    Opal::Game game;
    game.Init(1920, 1080, "Apophenia", Opal::RendererType::VULKAN);

    game.SetFramerateLock(60);
    //game.ToggleDebugInfo(true);

    game.Renderer->CreateOrthoCamera(1920, 1080, -1000, 1000);

    DialogueManager dialogue("../Dialogue/TestDialogue.xml");

    game.PushState<PlayState>();

    while(!game.ShouldEnd())
    {
        game.Tick();
    }
}