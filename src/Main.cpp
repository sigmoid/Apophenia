#include <iostream>

#include "../Opal/Game.h"

#include "Gamestate/PlayState.h"

int main(int argc, char **argv)
{
    Opal::Game game;
    game.Init(1920, 1080, "Apophenia", Opal::RendererType::VULKAN);

    game.Renderer->CreateOrthoCamera(1920, 1080, -1000, 1000);

    game.PushState<PlayState>();

    while(!game.ShouldEnd())
    {
        game.Tick();
    }
}